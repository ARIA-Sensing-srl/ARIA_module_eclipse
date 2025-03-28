/*
 * tbl_trig.h
 *
 *  Created on: Jan 29, 2024
 *      Author: ARIA Sensing
 */

#ifndef TBL_TRIG_H_
#define TBL_TRIG_H_
#include "fe_math.h"

/*
 * Access to tabulated version of sin and cos, the angle is normalized between 0-1
 * implemented for speedup the computation
 * */
float tbl_sin(float norm_phase);
float tbl_cos(float norm_phase);
complex_float tbl_exp(float norm_phase);
void tbl_init();


#endif /* TBL_TRIG_H_ */
