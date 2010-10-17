/*
 * entropy.h
 *
 *  Created on: Oct 17, 2010
 *      Author: mjackson
 */

#ifndef ENTROPY_H_
#define ENTROPY_H_

#ifdef __cplusplus
extern "C" {
#endif

void entropy(double ***probs, unsigned char **output, unsigned int rows, unsigned int cols, unsigned int classes);

#ifdef __cplusplus
}
#endif


#endif /* ENTROPY_H_ */
