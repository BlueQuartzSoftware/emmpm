//from http://www.songho.ca/opengl/gl_vbo.html

#ifndef _OPENCL_UTILS_H_
#define _OPENCL_UTILS_H_


#include <OpenCL/opencl.h>


char *file_contents(const char *filename, int *length);

cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID);
const char* oclErrorString(cl_int error);


#endif
