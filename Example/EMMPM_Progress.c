
#include <stdio.h>
#include "EMMPM_Progress.h"

void ProgressCallBack(char* message, float progress)
{
  printf("%s - %2.2f%% \n", message, progress);
}
