#if 0

/* allocate.h */
#ifndef em_allocate_h_
#define em_allocate_h_

#include "emmpm/common/EMMPMTypes.h"

#ifdef EMMPM_HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if EMMPM_HAVE_STDINT_H
#include <stdint.h>
#endif




#ifdef __cplusplus
extern "C"
{
#endif

void *get_spc(int num, size_t size);
void *mget_spc(int num, size_t size);
void **get_img(int wd, int ht, size_t size);
void EMMPM_free_img(void **pt);

void ***get_3d_img(int wd, int ht, int dp, size_t size);
void free_3d_img(void ***pt);

void *multialloc(size_t s, int d, ...);
void multifree(void *r,int d);

#ifdef __cplusplus
}
#endif


#endif /* _allocate.h_  */


#endif
