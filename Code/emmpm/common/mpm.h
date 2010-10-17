/*
 * mpm.h
 *
 *  Created on: Oct 17, 2010
 *      Author: mjackson
 */

#ifndef MPM_H_
#define MPM_H_

#include "emmpm/common/EMMPM_Constants.h"

#ifdef __cplusplus
extern "C" {
#endif

void mpm(unsigned char **y, unsigned char **xt, double **pr[], double beta,
           double *gamma, double *m, double *v, int rows, int cols, int n, int classes);

#ifdef __cplusplus
}
#endif



#endif /* MPM_H_ */
