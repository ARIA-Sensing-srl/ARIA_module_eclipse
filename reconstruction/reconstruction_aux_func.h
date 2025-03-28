/*
 * reconstruction_aux_func.h
 *
 *  Created on: Apr 23, 2024
 *      Author: andream
 */

#ifndef RECONSTRUCTION_RECONSTRUCTION_AUX_FUNC_H_
#define RECONSTRUCTION_RECONSTRUCTION_AUX_FUNC_H_

#include "math_utils.h"

complex_float rec_partCFcompute_asm(float16_t* pPart, int canvasPoints, int numStream, complex_float * pcfCplxCumSum, float * pcfCumAbsSum);


#endif /* RECONSTRUCTION_RECONSTRUCTION_AUX_FUNC_H_ */
