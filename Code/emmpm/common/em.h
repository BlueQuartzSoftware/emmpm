/*
 * em.h
 *
 *  Created on: Oct 17, 2010
 *      Author: mjackson
 */

#ifndef EM_H_
#define EM_H_

#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/common/EMMPM_Constants.h"


#ifdef __cplusplus
extern "C" {
#endif

struct _EMMPM_Inputs
{
    int emIterations;
    int mpmIterations;
    double beta;
    double gamma;
    int classes;
    unsigned int rows;
    unsigned int columns;
    char* input_file;
    char* output_file;
};
typedef struct _EMMPM_Inputs EMMPM_Inputs;



void EM_PerformEMLoops(EMMPM_Inputs* inputs,
                         unsigned char **y,
                         unsigned char **xt,
                         double **probs[MAX_CLASSES],
                         double gamma[MAX_CLASSES],
                         double m[MAX_CLASSES],
                         double v[MAX_CLASSES],
                         double N[MAX_CLASSES]);

#ifdef __cplusplus
}
#endif


#endif /* EM_H_ */
