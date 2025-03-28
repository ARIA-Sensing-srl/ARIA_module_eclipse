/**      License: ARIA Sensing -
 * wavelet.h
 *
 *  Created on: Feb 23, 2024
 *      Author: ARIA Sensing
 */


#ifndef WAVELET_H_
#define WAVELET_H_

/*
 * Computes the minimum output size, auxiliar buffer is this size + number of samples
 * */
int wlt_calc_min_size(int vector_size, int level);

/*
 * Compute the number of samples for each scale (pvect is an array pointer of level+1 elements
 * */
int wlt_calc_scale_info(int * pvect, int level, int size);

/*
 * Compute fast wavelet transform, filter kernel are considered symmetrical
 * paux size is size+numcoeffs
 * */
int wlt_fwt(float * psrc, float *pdst, int size, float * paux, const float* ph, const float *pg, int numcoeffs, int level);


/*
 * Compute fast wavelet transform, filter kernel are considered symmetrical
 * paux size is size+2*numcoeffs
 * */
int wlt_ifwt(float * psrc, float *pdst, int size, float * paux, const float* ph, const  float *pg, int numcoeffs, int level);

#define WLT_SUCCESS 0
#define WLT_ERROR -1

#endif /* WAVELET_H_ */
