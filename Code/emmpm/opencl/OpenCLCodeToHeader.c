/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
 // printf ("Starting OpenCLCodeToHeader.....\n");
  struct stat statbuf;
  FILE        *fh;
  int         file_len;
  char*       text;
  int i =0;
  fh = fopen(argv[1], "r");
  if (fh == 0)
      return EXIT_FAILURE;

  stat(argv[1], &statbuf);
  file_len = (int)statbuf.st_size;

  text = (char *) malloc(file_len + 1);
  fread(text, file_len, 1, fh);
  text[file_len] = '\0';

  fclose(fh);

//  printf("Generating: %s\n", argv[2]);
  fh = fopen(argv[2], "wb");
  if (fh == 0)
    return EXIT_FAILURE;

  fprintf(fh, "const char opencl_mpm_code[%d] = \n { ", file_len+1);
  for (i = 0; i < file_len; i++)
  {
    fprintf(fh, "%d, ", text[i]);
    if (i %16 == 15)
    {
      fprintf(fh, "\n");
    }
  }
  fprintf(fh, "0");


  fprintf(fh, "};");

  fclose(fh);

 // printf ("Ending OpenCLCodeToHeader.....\n");

  return EXIT_SUCCESS;

}


