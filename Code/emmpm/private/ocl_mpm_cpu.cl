

__kernel void mpm_init_yk(__global float* yk,
                          __global int* meta,
                          __global float* con,
                          __global unsigned char* y,
                          __global double* m,
                          __global double* v)
{

  int index = get_global_id(0);
  int lij;
  int ijd;
  int l;
  int ld;
  int classes = meta[3];
  int dims = meta[0];
  int d;
  int columns = meta[2];
  int rows = meta[1];



  for (l = 0; l < classes; l++)
  {
    lij = index*classes + l;
    yk[lij] = con[l];
    for (d = 0; d < dims; d++)
    {
      ld = dims * l + d;
      ijd = index + d;
      yk[lij] += ((y[ijd] - m[ld]) * (y[ijd] - m[ld]) / (-2.0 * v[ld]));
    }
  }



}
