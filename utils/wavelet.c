/*
 * wavelet.c
 *
 *  Created on: Feb 23, 2024
 *      Author: ARIA Sensing
 */

#include "wavelet.h"
#include "math.h"
#include "string.h"


int wlt_calc_min_size(int vector_size, int level){
	int samples = (1 << level);
	int frac = vector_size/samples; //integer division
	int ret = frac*samples ;
	if (ret != samples){
		ret += samples; //if not exact, add another block
	}
	return ret;
}

int wlt_calc_scale_info(int * pvect, int level, int size){
	if (size != wlt_calc_min_size(size, level))
		return WLT_ERROR;

	for (int i = level; i > 0; i--){
		size >>= 1;
		pvect[i] = size;
	}
	pvect[0] = pvect[1];
	return WLT_SUCCESS;
}

__attribute__((optimize(2)))
int wlt_fwt(float * psrc, float *pdst, int size, float * paux, const float* ph, const float *pg, int numcoeffs, int level){
	int offset = numcoeffs/2-1;
	int auxsize = size+numcoeffs;
	//copy into temporary buffer
	for (int i = 0; i< size; i++){
		paux[i+offset] = psrc[i];
	}
	for (int l = 0 ; l < level; l++){
		for (int i = 0; i< offset; i++)
			paux[i] = 0;
		for (int i = (offset+size); i< auxsize; i++)
			paux[i] = 0;
		for (int i =  0; i< size; i++){
			pdst[i]=0;
		}
		int len2 = size/2;
		for (int p = 0; p< len2; p++){
			for (int k = 0 ; k < numcoeffs; k++){
				pdst[p] += paux[p*2+k]*ph[k];
				pdst[p+len2] += paux[p*2+k]*pg[k];
			}
		}
		size = len2;
		for (int i = 0; i< size; i++){
			paux[i+offset] = pdst[i];
		}
	}
	return WLT_SUCCESS;
}

__attribute__((optimize(2)))
int wlt_ifwt(float * psrc, float *pdst, int size, float * paux, const float* ph, const float *pg, int numcoeffs, int level){
	float * tl;
	float * th;
	tl = paux;
	th = tl+(numcoeffs+size/2);
	int len = size / (1 << level);
	int offset = numcoeffs/4;
	memset((void*)paux, 0, sizeof(float)*(numcoeffs*2+size));

	for (int i = 0; i< len; i++){
		pdst[i] = psrc[i];
	}
	for (int i = len; i < size; i++){
		pdst[i]=0;
	}
	for (int l = 0; l< level; l++){
		for (int i = 0; i< len; i++){
			tl[offset+i] = pdst[i];
			th[offset+i] = psrc[i+len];
			pdst[i] = 0;
		}
		for(int s = 0 ; s < len; s++){
			for (int k =  0; k < numcoeffs; k+= 2){
				pdst[s*2] += ph[k]*tl[k/2+s] + pg[k]*th[k/2+s];
				pdst[s*2+1] += ph[k+1]*tl[k/2+s+1] + pg[k+1]*th[k/2+s+1];
			}
		}
		len <<= 1;
	}
	return WLT_SUCCESS;
}
