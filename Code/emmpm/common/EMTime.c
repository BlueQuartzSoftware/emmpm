///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h>
#include "EMTime.h"
#include "MSVCDefines.h"

static char em_log_time[128];
static char em_tif_time[128];

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* formattedTime(char* formatting, char* output)
{
  struct tm* t;
#ifdef _MSC_VER
#if _MSC_VER >= 1400
  struct tm time;
  errno_t tError;
#endif
#endif
  TimeType long_time;
  TimeFunc(&long_time);
  t = NULL;
#ifdef _MSC_VER
#if _MSC_VER < 1400
  t = _localtime64(&long_time);
#else
  t = &time;
  tError = _localtime64_s(&time, &long_time);
#endif
#else  // Non windows platforms
  t = localtime(&long_time);
#endif
  memset(output, 0, 128);
  snprintf(output, 128, "[%04d:%02d:%02d:%02d:%02d:%02d]", t->tm_year + 1900, t->tm_mon + 1,
    t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec  );
  return output;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* EM_LogTime() 
{
  return formattedTime("[%04d:%02d:%02d:%02d:%02d:%02d]", em_log_time);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* EM_TiffDateTime() 
{
  return formattedTime("%04d:%02d:%02d %02d:%02d:%02d", em_tif_time);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned long long int getMilliSeconds()
{
#ifdef _MSC_VER
  return (unsigned long long int)(clock());
#else
  struct timeval t1;
  EMMPM_GET_TIME_OF_DAY(&t1, NULL);
  unsigned long long int seconds = t1.tv_sec ;
  unsigned long long int microSec = t1.tv_usec ;
  seconds *= 1000;
  microSec /= 1000;
  return seconds + microSec; // Both in milliseconds at this point.
#endif
}
