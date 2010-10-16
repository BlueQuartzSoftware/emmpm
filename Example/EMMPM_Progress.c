
#include <stdio.h>
#include "EMMPM_Progress.h"

void ProgressCallBack(char* text, float progress)
{
  printf("%s - %2.2f%% \n", text, progress);
}
