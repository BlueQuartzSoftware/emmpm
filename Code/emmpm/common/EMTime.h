///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _EMTime_h_
#define _EMTime_h_

#include <EMMPM/Common/EMMPMConfiguration.h>
#if EMMPM_HAVE_TIME_H
#include <time.h>
#endif

#if defined (EMMPM_HAVE_SYS_TIME_GETTIMEOFDAY)
#if EMMPM_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#define EMMPM_GET_TIME_OF_DAY gettimeofday
#endif

#if defined (EMMPM_HAVE_TIME_GETTIMEOFDAY) || defined (_MSC_VER)
#if EMMPM_HAVE_TIME_H
#include <time.h>
#endif
#define EMMPM_GET_TIME_OF_DAY gettimeofday
#endif

#ifndef EMMPM_HAVE_TIME_GETTIMEOFDAY
#ifndef EMMPM_HAVE_SYS_TIME_GETTIMEOFDAY
#ifdef __MINGW32__
#ifndef GET_TIME_OF_DAY_DEFINITION
#define GET_TIME_OF_DAY_DEFINITION
//#warning no gettimeofday() function native to system. Using own implementation
//#endif

#include <windows.h>
#if EMMPM_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
void __stdcall GetSystemTimeAsFileTime(FILETIME*);

inline void EMMPM_gettimeofday(struct timeval* p, void* tz /* IGNORED */)
 {
  union {
     long long ns100; /*time since 1 Jan 1601 in 100ns units */
   FILETIME ft;
  } now;

    GetSystemTimeAsFileTime( &(now.ft) );
    p->tv_usec=(long)((now.ns100 / 10LL) % 1000000LL );
    p->tv_sec= (long)((now.ns100-(116444736000000000LL))/10000000LL);
 }

#define EMMPM_GET_TIME_OF_DAY EMMPM_gettimeofday
#endif /*  GET_TIME_OF_DAY_WARNING */
#else
#error Your system does not have a gettimeofday() function. Please contact the author of this library for advice.
#endif /* MinGW case */
#endif /* HAVE_SYS_TIME_GETTIMEOFDAY */
#endif /* HAVE_TIME_GETTIMEOFDAY */

#ifdef _MSC_VER
#if _MSC_VER < 1400
#define TimeType __time64_t
#define TimeFunc _time64
#else
#define TimeType __time64_t
#define TimeFunc _time64
#endif
#else
#define TimeType time_t
#define TimeFunc time
#endif

#ifdef __cplusplus
 extern "C" {
 #endif

/**
* @brief Returns a Formatted String of the current Date/Time for logging
* purpose.
* @return A string of the current date/time
*/
char* EM_LogTime();

/**
 * Returns a date/time string suitable for tiff tags.
 * @return
 */
char* EM_TiffDateTime() ;

/**
 * @brief returns the number of milliseconds from a platform specified time.
 */
unsigned long long int getMilliSeconds();


#ifdef __cplusplus
}
#endif



#endif /* _EMTime.h_  */
