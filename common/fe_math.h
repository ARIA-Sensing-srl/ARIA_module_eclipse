/**
 * @file
 * fe_math.h
 * @brief Auxiliary math files, definition of types used for elaboration
 *
 *  Created on: Dec 29, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef COMMON_FE_MATH_H_
#define COMMON_FE_MATH_H_

	#include <stdint.h>
	#include <math.h>

	typedef struct _point2d
	{
		uint16_t	x;
		uint16_t	y;
	}point_2d;

	typedef struct _complex_int8
	{
		int8_t real;
		int8_t imag;
	}complex_int8;

	typedef struct _complex_int32
	{
		int32_t real;
		int32_t imag;
	}complex_int32;

	typedef struct _complex_float_t{
		float real;
		float imag;
	}complex_float;

	typedef int16_t q15_t;
	typedef int32_t q31_t;
	typedef int8_t q7_t;


#endif /* COMMON_FE_MATH_H_ */
