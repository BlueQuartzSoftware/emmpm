__kernel void mpm_init_yk(__global float* yk,
                          __global int* meta,
                          __global float* con,
                          __global unsigned char* y,
                          __global double* m,
                          __global double* v,
                            int* totalPixelCount,
                            int* workPixelCount)
{
  int index = get_global_id(0);
  int lij;
  int ijd;
  int l;
  int ld;
  int classes = meta[3];
  int dims = meta[0];
  int d;
  int i;
  int j;
  int rows = meta[1];
  int cols = meta[2];

  int startIndex = index*workPixelCount[0];
  int endIndex = startIndex + workPixelCount[0];
  int xStart = startIndex % meta[2];
  int yStart = startIndex / meta[2];

  int xEnd = endIndex % meta[2];
  int yEnd = endIndex / meta[2];

  printf("get_global_id: %d\n", index);
  printf("totalPixelCount: %d\n", totalPixelCount[0]);
  printf("workPixelCount: %d\n", workPixelCount[0]);
  printf("startIndex: %d\n", startIndex);
  printf("endIndex: %d\n", endIndex);
  printf("xStart: %d\n", xStart);
  printf("yStart: %d\n", yStart);
  printf("xEnd: %d\n", xEnd);
  printf("yEnd: %d\n", yEnd);


  for (i = yStart; i < yEnd; i++)
  {
    for (j = xStart; j < xEnd; j++)
    {
      for (l = 0; l < classes; l++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        yk[lij] = con[l];
        for (d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          ijd = (dims * cols * i) + (dims * j) + d;
          yk[lij] += ((y[ijd] - m[ld]) * (y[ijd] - m[ld]) / (-2.0 * v[ld]));
        }
      }
    }
  }
}

__kernel void mpm_loop(__global int* meta,
                          __global double* xt,
                          __global double* ns,
                          __global double* ew,
                          __global double* sw,
                          __global double* nw,
                          __global float* yk,
                          __global float workingBeta,
                          __global float beta_c,
                          __global double* ccost,
                          __global double* w_gamma,
                          __global double* probs,
                          __global float* rands
                          )
{

  /* We are running over the width x height dimensions so the index is the index
   * into that type of array
   */
  int ij = get_global_id(0);
  float sum = 0;
  int l = 0;
  int dims = meta[0];
  int classes = meta[3];
  int prior = 0;
  float edge = 0.0;
  int i = 0;
  int j = 0;
  int i1j1;
  int lij;
  int columns = meta[2];
  int rows = meta[1];
  float x;
  float current;
  float post[16];

  size_t nsCols = columns - 1;
  size_t ewCols = columns;
  size_t swCols = columns - 1;
  size_t nwCols = columns - 1;



  if (ij != 0)
  {
    i = ij % meta[2];
    j = ij / meta[2];
  }

  sum = 0.0;
  for (l = 0; l < classes; l++)
  {
    /* edge penalties (in both x and y) */
    prior = 0;
    edge = 0;
    if (i - 1 >= 0)
    {
      if (j - 1 >= 0)
      {
        i1j1 = (columns*(i-1))+j-1;
        if (xt[i1j1] != l)
        {
          prior++;
          i1j1 = (swCols*(i-1))+j-1;
          edge += sw[i1j1];
        }
      }

      //Mark1
      i1j1 = (columns*(i-1))+j;
      if (xt[i1j1] != l)
      {
        prior++;
        i1j1 = (ewCols*(i-1))+j;
        edge += ew[i1j1];
      }
      //mark2
      if (j + 1 < columns)
      {
        i1j1 = (columns*(i-1))+j+1;
        if (xt[i1j1] != l)
        {
          prior++;
          i1j1 = (nwCols*(i-1))+j;
          edge += nw[i1j1];
        }
      }
    }

    //mark3
    if (i + 1 < rows)
    {
      if (j - 1 >= 0)
      {
        i1j1 = (columns*(i+1))+j-1;
        if (xt[i1j1] != l)
        {
          prior++;
          i1j1 = (nwCols*(i))+j-1;
          edge += nw[i1j1];
        }
      }
      //mark4
      i1j1 = (columns*(i+1))+j;
      if (xt[i1j1] != l)
      {
        prior++;
        i1j1 = (ewCols*(i))+j;
        edge += ew[i1j1];
      }
      //mark5
      if (j + 1 < columns)
      {
        i1j1 = (columns*(i+1))+j+1;
        if (xt[i1j1] != l)
        {
          prior++;
          i1j1 = (swCols*(i))+j;
          edge += sw[i1j1];
        }
      }
    }
    //mark6
    if (j - 1 >= 0)
    {
      i1j1 = (columns*(i))+j-1;
      if (xt[i1j1] != l)
      {
        prior++;
        i1j1 = (nsCols*(i))+j-1;
        edge += ns[i1j1];
      }
    }
    //mark7
    if (j + 1 < columns)
    {
      i1j1 = (columns*(i))+j+1;
      if (xt[i1j1] != l)
      {
        prior++;
        i1j1 = (nsCols*(i))+j;
        edge += ns[i1j1];
      }
    }
    lij = (columns * rows * l) + (columns * i) + j;
    post[l] = exp(yk[lij] - (workingBeta * (double)prior) - edge - (beta_c * ccost[lij]) - w_gamma[l]);
    sum += post[l];
  }

  current = 0;
  for (l = 0; l < classes; l++)
  {
    lij = (columns * rows * l) + (columns * i) + j;
    ij = (columns*i)+j;
    if ((rands[ij] >= current) && (rands[ij] <= (current + post[l] / sum)))
    {
      xt[ij] = l;
      probs[lij] += 1.0;
    }
    current += post[l] / sum;
  }

}
