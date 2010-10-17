/*
 * EMMPM.h
 *
 *  Created on: Oct 17, 2010
 *      Author: mjackson
 */

#ifndef EMMPM_H_
#define EMMPM_H_


#include "emmpm/common/em.h"

typedef void EMMPM_ExtendedInputs;


#ifdef __cplusplus
extern "C" {
#endif


void Run_EMMPM(EMMPM_Inputs* inputs, EMMPM_ExtendedInputs* extInputs);



#ifdef __cplusplus
}
#endif


#endif /* EMMPM_H_ */
