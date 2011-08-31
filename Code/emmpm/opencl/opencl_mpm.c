
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "emmpm/common/EMMPM_Math.h"
#include "emmpm/common/random.h"
#include "emmpm/common/EMTime.h"
#include "emmpm/public/EMMPM.h"
#include "emmpm/private/curvature_mpm.h"

#include "emmpm/opencl/OpenCLUtils.h"
//-- Include the generated header that contains the kernel program
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

#define OCL_CREATE_BUFFER_SET_ARG(count, in_var, cl_var, mem_flags, arg)\
    cl_mem cl_var = clCreateBuffer(m_compute_context,\
                                        (cl_mem_flags)(mem_flags),\
                                         count, in_var, &return_value);\
    err  = clSetKernelArg(m_compute_kernel, arg, sizeof(cl_mem), &cl_var);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ocl_acv_mpm(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{

  // Initialize the MPM Variables
   float* m_host_yk;
   float sqrt2pi, current, con[EMMPM_MAX_CLASSES];
   float x, post[EMMPM_MAX_CLASSES], sum, edge;
   int k, l, prior;
   int i, j, d;
   size_t ld, ijd, ij, lij, i1j1;
   size_t probsSize = data->columns * data->rows * data->classes;

   char msgbuff[256];
   float totalLoops;
   float currentLoopCount = 0.0;

   size_t nsCols = data->columns - 1;
   size_t ewCols = data->columns;
   size_t swCols = data->columns - 1;
   size_t nwCols = data->columns - 1;

   memset(post, 0, EMMPM_MAX_CLASSES * sizeof(float));
   memset(con, 0,  EMMPM_MAX_CLASSES * sizeof(float));

   totalLoops = (float)(data->emIterations * data->mpmIterations + data->mpmIterations);
   memset(msgbuff, 0, 256);
   data->progress++;
   sqrt2pi = sqrt(2.0 * M_PI);

  int return_value;
  int err;

//  size_t src_len = 0;
//  int vectorized = 1;
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
  printf("Width x Height x Classes: %d x %d x %d \n", data->columns, data->rows, data->classes);
  int toalPixelCount = data->rows * data->columns;
  int workPixelCount = toalPixelCount/m_compute_units; // What if this does NOT divide evenly??
  printf("Total Pixels: %d\n", toalPixelCount);
  printf("CPU Init: m_compute_units: %d\n", m_compute_units);
  // How many pixels are we going to compute in each workgroup, which is to say , each CPU Core.
  printf("Pixels Per Compute Unit: %d\n", workPixelCount);



//
//  return_value = LoadFileIntoString(EMMPM_OPENCL_SOURCE_FILE, &source, &src_len);
//  if (return_value)
//      return;
  const char* program[1] = {opencl_mpm_code};
  m_compute_program = clCreateProgramWithSource(m_compute_context,
                                                1, // There is a single char array
                                                (const char**)(&program),
                                                NULL, // All strings are null terminated
                                                &return_value);
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



  unsigned long long int millis = EMMPM_getMilliSeconds();

  // Initialize the 'con' variable
  for (l = 0; l < data->classes; l++)
  {
   con[l] = 0;
   for (d = 0; d < data->dims; d++)
   {
     ld = data->dims * l + d;
     con[l] += -log(sqrt2pi * sqrt(data->v[ld]));
   }
  }

  // Initialize the probablities to Zero - This could probably be merged into the
  // OpenCL code or we could try SSE4 based code
  // initialize the random numbers:
  size_t totalPixels = data->columns * data->rows;
  float* rands = (float*)(malloc(sizeof(float) * totalPixels));
  for (i = 0; i < data->rows; i++)
  {
   for (j = 0; j < data->columns; j++)
   {
     rands[(data->columns * i) + j] = genrand_real2(data->rngVars);
     for (l = 0; l < data->classes; l++)
     {
       lij = (data->columns * data->rows * l) + (data->columns * i) + j;
       data->probs[lij] = 0.0f;
     }
   }
  }


  /* *** Start OpenCL Code Section *******/
  size_t global_dim;
  size_t local_dim;

  local_dim  = 1;
  global_dim = m_compute_units;

  // Compute which row each compute unit will start on. This ensures we compute
  // every pixel of the image. The last compute unit may not be completely used
  // but should be pretty close
  int* rowStartValues = (int*)(malloc(sizeof(int) * (m_compute_units + 1)));
  int rowComputeIncrement = data->rows / m_compute_units;
  j = 0;
  i = 0;
  for(i = 0; i < m_compute_units;i++)
  {
    rowStartValues[i] = j;
  //  printf("rowStartValues[%d] = %d\n", i, rowStartValues[i]);
    j = j + rowComputeIncrement;
  }
  rowStartValues[m_compute_units] = data->rows;
 // printf("rowStartValues[%d] = %d\n", i, rowStartValues[i]);

  count = data->dims * data->rows * data->columns * data->classes;
  m_host_yk = (float*)malloc(count* sizeof(float));

  // Create an OpenCL Device Buffer to store the yk values
  OCL_CREATE_BUFFER_SET_ARG(sizeof(float)*count, m_host_yk, m_device_yk, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 0);

  int m_host_meta[4] = {data->dims, data->rows, data->columns, data->classes};
  OCL_CREATE_BUFFER_SET_ARG(4*sizeof(int), m_host_meta, m_device_meta, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 1);

  OCL_CREATE_BUFFER_SET_ARG(EMMPM_MAX_CLASSES*sizeof(float), con, m_device_con, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 2);
  OCL_CREATE_BUFFER_SET_ARG((data->dims*data->rows*data->columns), data->y, m_device_y, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 3);
  OCL_CREATE_BUFFER_SET_ARG(data->dims*data->classes*sizeof(double), data->m, m_device_m, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 4);
  OCL_CREATE_BUFFER_SET_ARG(data->dims*data->classes*sizeof(double), data->v, m_device_v, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 5);
  OCL_CREATE_BUFFER_SET_ARG(m_compute_units*sizeof(int), rowStartValues, m_device_rowStartValues, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 6);



  /* *** Put the program into the Queue to be executed */
  err = clEnqueueNDRangeKernel(m_compute_command, m_compute_kernel, 1, NULL, &global_dim, &local_dim, 0, NULL, NULL);

  /* ** Wait for the execution to complete */
  err |= clFinish(m_compute_command);
  if (err != CL_SUCCESS)
      { free(rands); free(m_host_yk); free(rowStartValues); return; }

  printf("OpenCL Millis to complete initialization: %llu \n", EMMPM_getMilliSeconds() - millis);


/* * -----------------------------------------------------------------------------
     Start the OpenCL section for the main mpm loops
  ----------------------------------------------------------------------------- */
  // Create a new kernel for the loop
  m_compute_kernel = clCreateKernel(m_compute_program, "mpm_loop", &return_value);
  if (!m_compute_kernel)
  { free(rands); free(m_host_yk); free(rowStartValues); return; }



  /* Perform the MPM loops */
   for (k = 0; k < data->mpmIterations; k++)
   {
     data->currentMPMLoop = k;
     if (data->cancel) { data->progress = 100.0; break; }
     data->inside_mpm_loop = 1;
     millis = EMMPM_getMilliSeconds();
#if 0
     printf("OpenCL Starting MPM Loop.... \n");
     OCL_CREATE_BUFFER_SET_ARG(4*sizeof(int), m_host_meta, m_device_meta, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 0);
     OCL_CREATE_BUFFER_SET_ARG(data->columns*data->rows, data->xt, m_device_xt, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 1);
     OCL_CREATE_BUFFER_SET_ARG((data->columns-1)*data->rows*sizeof(double), data->ns, m_device_ns, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 2);
     OCL_CREATE_BUFFER_SET_ARG((data->columns)*data->rows*sizeof(double), data->ew, m_device_ew, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 3);
     OCL_CREATE_BUFFER_SET_ARG((data->columns-1)*data->rows*sizeof(double), data->sw, m_device_sw, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 4);
     OCL_CREATE_BUFFER_SET_ARG((data->columns-1)*data->rows*sizeof(double), data->nw, m_device_nw, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 5);
     OCL_CREATE_BUFFER_SET_ARG(sizeof(float)*count, m_host_yk, m_device_yk, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 6);
     OCL_CREATE_BUFFER_SET_ARG(1*sizeof(float), &(data->workingBeta), m_device_workingBeta, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 7);
     OCL_CREATE_BUFFER_SET_ARG(1*sizeof(float), &(data->beta_c), m_device_beta_c, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 8);
     OCL_CREATE_BUFFER_SET_ARG((data->classes*data->rows*data->columns)*sizeof(double), data->ccost, m_device_ccost, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 9);
     OCL_CREATE_BUFFER_SET_ARG(EMMPM_MAX_CLASSES*sizeof(double), data->w_gamma, m_device_w_gamma, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 10);
     OCL_CREATE_BUFFER_SET_ARG((data->rows*data->columns*data->classes)*sizeof(double), data->probs, m_device_probs, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 11);
     OCL_CREATE_BUFFER_SET_ARG((data->rows*data->columns)*sizeof(float), rands, m_device_rands, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 12);
     OCL_CREATE_BUFFER_SET_ARG(m_compute_units*sizeof(int), rowStartValues, m_device_rowStartValues, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 13);


     /* *** Put the program into the Queue to be executed */
     err = clEnqueueNDRangeKernel(m_compute_command, m_compute_kernel,
                                  1, NULL, &global_dim, &local_dim, 0, NULL, NULL);
     if (err != CL_SUCCESS)
     {
       printf("OpenCL Error Enqueueing Kernel\n");
       break;
     }
     /* ** Wait for the execution to complete */
     err |= clFinish(m_compute_command);
     if (err != CL_SUCCESS)
         { free(rands); free(m_host_yk); free(rowStartValues); return; }


#else
     printf("Serial Starting MPM Loop.... \n");
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
#endif
     printf("Millis to complete loop: %llu \n", EMMPM_getMilliSeconds() - millis);
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
   free(rands);
   free(rowStartValues);

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


