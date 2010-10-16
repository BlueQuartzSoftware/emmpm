
#include <stdio.h>
#include <stdlib.h>

/* Defines all of the public API functions that can be called by outside programs. */
#include "EMMPM_Public.h"

/* Defines our header that defines the 'ProgressCallBack' that takes the progress 
* information and does something with it */
#include "EMMPM_Progress.h"


/**
* @param argc
* @param argv
* @return
*/
int main (int argc, char const *argv[])
{
  /* Declare local Variables */
  int err;
  EMMPM_Parameters* params;
  
  /* Initialize local variables */
  err = 0;
  printf ("Starting EM/MPM \n");
  
  /* Set a callback function to use for progress updates */
  EMMPM_setProgressFunction( &ProgressCallBack );
  
  /* Allocate the EMMPM_Parameters structure */
  params = Allocate_EMMPM_Parameters();
  if (NULL == params)
  {
    return 1;
  }
  
  /* Read the input image from somewhere */
  
  /* Initialize the EMMPM_Parameters structure with values from the command line */
  
  /* Execute the EMMPM algorithm */
  err = EMMPM_execute(params);
  if (err != 0)
  {
    /* Some error occurred during the segmentation */
  }
  
  /* Clean up other memory such as input/output image memory */
  
  /* Clean up allocated memory for the EMMPM_Parameters */
  Free_EMMPM_Parameters(params);
  return err;
}
