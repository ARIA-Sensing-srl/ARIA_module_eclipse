/*
 * utils.h
 *
 *  Created on: Jan 24, 2024
 *      Author: ARIA Sensing
 */

#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

#include "stdint.h"
typedef int16_t float16_t;

typedef struct _cplx_float16_t{
	float16_t x;
	float16_t y;
}cplx_float16_t;

void utils_conv_float2half(float *, float16_t * , int);
void utils_conv_half2float(float16_t * ,float *,  int);
void utils_conv_float2int(float *, int32_t * , int);
void utils_conv_int2float(int32_t * , float *, int);


#endif /* MATH_UTILS_H_ */
