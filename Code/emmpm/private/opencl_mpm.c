
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "emmpm/common/EMMPM_Math.h"
#include "emmpm/common/random.h"
#include "emmpm/common/EMTime.h"
#include "emmpm/ocl/OpenCLUtils.h"
#include "emmpm/public/EMMPM.h"
#include "emmpm/private/curvature_mpm.h"
#include "emmpm/opencl/ocl_mpm_cpu.h"






cl_platform_id    platform;
cl_device_id      m_compute_device_id;             // compute device id
unsigned int      m_compute_units;

unsigned int      m_device_count;
char              m_device_name[1024];
cl_context        m_compute_context;                 // compute context
cl_command_queue  m_compute_command;          // compute command queue
cl_program        m_compute_program;                 // compute program
cl_kernel         m_compute_kernel;                   // compute m_compute_kernel
int               m_device_index;
cl_mem input;                       // device memory used for the input array
cl_mem output;                      // device memory used for the output array


#define OPEN_CL_CPU 0
#define OPEN_CL_GPU 1
#define EMMPM_OPENCL_SOURCE_FILE "ocl_mpm_cpu.cl"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LoadFileIntoString(const char *filename, char **text, size_t *len)
{
    struct stat statbuf;
    FILE        *fh;
    int         file_len;

    fh = fopen(filename, "r");
    if (fh == 0)
        return -1;

    stat(filename, &statbuf);
    file_len = (int)statbuf.st_size;
    *len = file_len;
    *text = (char *) malloc(file_len + 1);
    fread(*text, file_len, 1, fh);
    (*text)[file_len] = '\0';

    fclose(fh);
    return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ocl_acv_mpm(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{

  int return_value;
  int err;
  char *source;
  size_t src_len;
  int vectorized = 1;
  unsigned int count = 0;
  int threaded = 1; // We want the maximum threads the CPU can handle
  return_value = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_CPU, 1, &m_compute_device_id, &m_device_count);
  if (return_value)
     return;

  m_compute_context = clCreateContext(0, m_device_count, &m_compute_device_id, 0, 0, &return_value);
  if (!m_compute_context)
     return;

  m_compute_command = clCreateCommandQueue(m_compute_context, m_compute_device_id, 0, &return_value);
  if (!m_compute_command)
     return;

  size_t returned_size;
  unsigned int compute_units;

  clGetDeviceInfo(m_compute_device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(unsigned int), &compute_units,  &returned_size);
  m_compute_units = threaded ? compute_units : 1;
  printf("CPU Init: m_compute_units: %d\n", m_compute_units);

//
//  return_value = LoadFileIntoString(EMMPM_OPENCL_SOURCE_FILE, &source, &src_len);
//  if (return_value)
//      return;

  m_compute_program = clCreateProgramWithSource(m_compute_context, 1, (const char**) & opencl_mpm_code, 0, &return_value);
  if (!m_compute_program)
      return;

  return_value = clBuildProgram(m_compute_program, m_device_count, &m_compute_device_id, 0, NULL, NULL);
  if (return_value != CL_SUCCESS)
  {
      size_t length = 0;
      char info_log[2000];
      clGetProgramBuildInfo(m_compute_program, m_compute_device_id, CL_PROGRAM_BUILD_LOG, 2000, info_log, &length);
      fprintf(stderr, "%s\n", info_log);
      return;
  }

  m_compute_kernel = clCreateKernel(m_compute_program, "mpm_init_yk", &return_value);
  if (!m_compute_kernel)
      return;

  free(source);

  // Initialize the MPM Variables
  float* m_host_yk;
  float sqrt2pi, current, con[EMMPM_MAX_CLASSES];
  float x, post[EMMPM_MAX_CLASSES], sum, edge;
  int k, l, prior;
  int i, j, d;
  size_t ld, ijd, ij, lij, i1j1;

  char msgbuff[256];
  float totalLoops;
  float currentLoopCount = 0.0;

  size_t nsCols = data->columns - 1;
  size_t ewCols = data->columns;
  size_t swCols = data->columns - 1;
  size_t nwCols = data->columns-1;

  memset(post, 0, EMMPM_MAX_CLASSES * sizeof(float));
  memset(con, 0,  EMMPM_MAX_CLASSES * sizeof(float));

  totalLoops = (float)(data->emIterations * data->mpmIterations + data->mpmIterations);
  memset(msgbuff, 0, 256);
  data->progress++;
  sqrt2pi = sqrt(2.0 * M_PI);

  unsigned long long int millis = EMMPM_getMilliSeconds();

  for (l = 0; l < data->classes; l++)
  {
    con[l] = 0;
    for (d = 0; d < data->dims; d++)
    {
      ld = data->dims * l + d;
      con[l] += -log(sqrt2pi * sqrt(data->v[ld]));
    }
  }
  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      for (l = 0; l < data->classes; l++)
      {
        lij = (data->columns * data->rows * l) + (data->columns * i) + j;
        data->probs[lij] = 0;
      }
    }
  }


  /* *** Start OpenCL Code Section *******/
  size_t global_dim[2];
  size_t local_dim[2];

  local_dim[0]  = 1;
  local_dim[1]  = 1;

  global_dim[0] = m_compute_units;
  global_dim[1] = 1;

  count = data->dims * data->rows * data->columns * data->classes;
  m_host_yk = (float*)malloc(count* sizeof(float));

  // Create an OpenCL Device Buffer to store the yk values
  cl_mem m_device_yk = clCreateBuffer(m_compute_context,
                                      (cl_mem_flags)(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR),
                                      sizeof(float) * count, m_host_yk, &return_value);
  err  = clSetKernelArg(m_compute_kernel, 0, sizeof(cl_mem), &m_device_yk);

  int m_host_meta[4] = {data->dims, data->rows, data->columns, data->classes};
  cl_mem m_device_meta = clCreateBuffer(m_compute_context,
                                      (cl_mem_flags)(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR),
                                       4, m_host_meta, &return_value);
  err  = clSetKernelArg(m_compute_kernel, 1, sizeof(cl_mem), &m_device_meta);

  cl_mem m_device_con = clCreateBuffer(m_compute_context,
                                      (cl_mem_flags)(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR),
                                       EMMPM_MAX_CLASSES, con, &return_value);
  err  = clSetKernelArg(m_compute_kernel, 2, sizeof(cl_mem), &m_device_con);

  cl_mem m_device_y = clCreateBuffer(m_compute_context,
                                      (cl_mem_flags)(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR),
                                       EMMPM_MAX_CLASSES, data->y, &return_value);
  err  = clSetKernelArg(m_compute_kernel, 3, sizeof(cl_mem), &m_device_y);

  cl_mem m_device_m = clCreateBuffer(m_compute_context,
                                      (cl_mem_flags)(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR),
                                       EMMPM_MAX_CLASSES, data->m, &return_value);
  err  = clSetKernelArg(m_compute_kernel, 4, sizeof(cl_mem), &m_device_m);

  cl_mem m_device_v = clCreateBuffer(m_compute_context,
                                      (cl_mem_flags)(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR),
                                       EMMPM_MAX_CLASSES, data->v, &return_value);
  err  = clSetKernelArg(m_compute_kernel, 5, sizeof(cl_mem), &m_device_v);


  // How many pixels are we going to compute in each workgroup, which is to say , each CPU Core.
  int m_body_count_per_work_group = (data->columns * data->rows) / m_compute_units;
  printf("m_body_count_per_work_group: %d\n", m_body_count_per_work_group);



  err = clEnqueueNDRangeKernel(m_compute_command, m_compute_kernel,
                               2, NULL, global_dim, local_dim, 0, NULL, NULL);

  err |= clFinish(m_compute_command);
  if (err != CL_SUCCESS)
      return;

  printf("OpenCL Millis to complete initialization: %llu \n", EMMPM_getMilliSeconds() - millis);


  /* Perform the MPM loops */
   for (k = 0; k < data->mpmIterations; k++)
   {
     data->currentMPMLoop = k;
     if (data->cancel) { data->progress = 100.0; break; }
     data->inside_mpm_loop = 1;


     for (i = 0; i < data->rows; i++)
     {
       for (j = 0; j < data->columns; j++)
       {
         ij = (data->columns * i) + j;
         sum = 0;
         for (l = 0; l < data->classes; l++)
         {
           /* edge penalties (in both x and y) */
           prior = 0;
           edge = 0;
           if (i - 1 >= 0)
           {
             if (j - 1 >= 0)
             {
               i1j1 = (data->columns*(i-1))+j-1;
               if (data->xt[i1j1] != l)
               {
                 prior++;
                 i1j1 = (swCols*(i-1))+j-1;
                 edge += data->sw[i1j1];
               }
             }

             //Mark1
             i1j1 = (data->columns*(i-1))+j;
             if (data->xt[i1j1] != l)
             {
               prior++;
               i1j1 = (ewCols*(i-1))+j;
               edge += data->ew[i1j1];
             }
             //mark2
             if (j + 1 < data->columns)
             {
               i1j1 = (data->columns*(i-1))+j+1;
               if (data->xt[i1j1] != l)
               {
                 prior++;
                 i1j1 = (nwCols*(i-1))+j;
                 edge += data->nw[i1j1];
               }
             }
           }

           //mark3
           if (i + 1 < data->rows)
           {
             if (j - 1 >= 0)
             {
               i1j1 = (data->columns*(i+1))+j-1;
               if (data->xt[i1j1] != l)
               {
                 prior++;
                 i1j1 = (nwCols*(i))+j-1;
                 edge += data->nw[i1j1];
               }
             }
             //mark4
             i1j1 = (data->columns*(i+1))+j;
             if (data->xt[i1j1] != l)
             {
               prior++;
               i1j1 = (ewCols*(i))+j;
               edge += data->ew[i1j1];
             }
             //mark5
             if (j + 1 < data->columns)
             {
               i1j1 = (data->columns*(i+1))+j+1;
               if (data->xt[i1j1] != l)
               {
                 prior++;
                 i1j1 = (swCols*(i))+j;
                 edge += data->sw[i1j1];
               }
             }
           }
           //mark6
           if (j - 1 >= 0)
           {
             i1j1 = (data->columns*(i))+j-1;
             if (data->xt[i1j1] != l)
             {
               prior++;
               i1j1 = (nsCols*(i))+j-1;
               edge += data->ns[i1j1];
             }
           }
           //mark7
           if (j + 1 < data->columns)
           {
             i1j1 = (data->columns*(i))+j+1;
             if (data->xt[i1j1] != l)
             {
               prior++;
               i1j1 = (nsCols*(i))+j;
               edge += data->ns[i1j1];
             }
           }
           lij = (data->columns * data->rows * l) + (data->columns * i) + j;
           post[l] = exp(m_host_yk[lij] - (data->workingBeta * (double)prior) - edge - (data->beta_c * data->ccost[lij]) - data->w_gamma[l]);
           sum += post[l];
         }
         x = genrand_real2(data->rngVars);
         current = 0;
         for (l = 0; l < data->classes; l++)
         {
           lij = (data->columns * data->rows * l) + (data->columns * i) + j;
           ij = (data->columns*i)+j;
           if ((x >= current) && (x <= (current + post[l] / sum)))
           {
             data->xt[ij] = l;
             data->probs[lij] += 1.0;
           }
           current += post[l] / sum;
         }
       }
     }

     EMMPM_ConvertXtToOutputImage(data, callbacks);
     if (callbacks->EMMPM_ProgressFunc != NULL)
     {
       data->currentMPMLoop = k;
       snprintf(msgbuff, 256, "MPM Loop %d", data->currentMPMLoop);
       callbacks->EMMPM_ProgressFunc(msgbuff, data->progress);
     }
     if (NULL != callbacks->EMMPM_ProgressStatsFunc)
     {
       currentLoopCount = data->mpmIterations * data->currentEMLoop + data->currentMPMLoop;
       data->progress = currentLoopCount / totalLoops * 100.0;
       callbacks->EMMPM_ProgressStatsFunc(data);
     }

   }
   data->inside_mpm_loop = 0;

   if (!data->cancel)
   {
   /* Normalize probabilities */
     for (i = 0; i < data->rows; i++)
     {
       for (j = 0; j < data->columns; j++)
       {
         for (l = 0; l < data->classes; l++)
         {
           lij = (data->columns * data->rows * l) + (data->columns * i) + j;
           data->probs[lij] = data->probs[lij] / (double)data->mpmIterations;
         }
       }
     }
   }

   /* Clean Up Memory */
   free(m_host_yk);


}





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//int initGPU()
//{
//  int return_value;
//  unsigned int count = 0;
//  unsigned int i = 0;
//
//  return_value = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 4, m_compute_device_id, &count);
//    if (return_value)
//        return -1;
//
//
//
//    printf("Found %d GPU devices...\n", count);
//    m_device_count = (count > m_device_count) ? m_device_count : count;
//
//    if (m_device_index >= 0)
//    {
//        m_device_count = 1;
//        i = m_device_index;
//        size_t returned_size = 0;
//        char name[1024] = {0};
//        char vendor[1024] = {0};
//
//        clGetDeviceInfo(m_compute_device_id[i], CL_DEVICE_NAME, sizeof(name), &name, &returned_size);
//        clGetDeviceInfo(m_compute_device_id[i], CL_DEVICE_VENDOR, sizeof(vendor), &vendor, &returned_size);
//
//        sprintf(m_device_name, "%s", name);
//        printf("Using Device[%d]: %s\n", i, m_device_name);
//
//        unsigned int compute_units;
//        clGetDeviceInfo(m_compute_device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(unsigned int), &compute_units,  &returned_size);
//        printf("GPU Init: m_compute_units: %d\n", compute_units);
//
//        m_compute_device_id = m_compute_device_id[i];
//        m_compute_context = clCreateContext(0, 1, &m_compute_device_id, 0, 0, &return_value);
//        m_compute_command = clCreateCommandQueue(m_compute_context, m_compute_device_id, 0, &return_value);
//        if (!m_compute_command)
//            return -2;
//    }
//    else
//    {
//        m_compute_context = clCreateContext(0, m_device_count, m_compute_device_id, 0, 0, &return_value);
//        for (i = 0; i < m_device_count; i++)
//        {
//            m_compute_commands[i] = clCreateCommandQueue(m_compute_context, m_compute_device_id[i], 0, &return_value);
//            if (!m_compute_commands[i])
//                return -2;
//        }
//    }
//
//
//  return return_value;
//}


