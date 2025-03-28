/**
 * @file processing.h
 * @brief stream pre-processing routines
 *
 * Processing function could be accessed separately or a full processing could be executed\n
 * Some function are assembler optimized to exploit specific processor's features\n
 *
 *  Created on: Jan 16, 2023\n
 *  Author: ARIA Sensing\n
 *
 */



#ifndef PROCESSING_H_
#define PROCESSING_H_


#include "fe_math.h"

#define proc_iir_biquad proc_iir_biquad_asm


typedef struct _declattuer_constants_t{
	float normTau;
	float alpha;
	float beta;
}declattuer_constants_t;

typedef struct _proc_stream_t{
	complex_float * praw;
	complex_float * pclutter;
	complex_float * pout;
	declattuer_constants_t * pdecl;
	int size;
	uint8_t clutter_empty;
}proc_stream_t;



/**
 * Biquadratic filter implementation
 * @param pSrc input array
 * @param pDst output array
 * @param numData number of samples
 * @param coeffs filter coefficients arranged as bs0, bs1, bs2, as1, as2 (as0 implicitly 1, s is the section)
 * @param numSections number of sections
 * @param stride samples access stride
 * @param reverse compute filter in reverse time order
 */
void proc_iir_biquad(const float *pSrc, float *pDst, int numData, const float *coeffs, int numSections, int stride, int reverse);

/**
 * Zero delay biquadratic filter (equivalent to Octave's filtfilt)
 * @param pSrc  input array
 * @param pDst output array
 * @param numData number of samples
 * @param coeffs filter coefficients arranged as bs0, bs1, bs2, as1, as2 (as0 implicitly 1, s is the section)
 * @param numSections number of sections
 * @param stride samples access stride
 */
void proc_filtfilt_biquad(const float *pSrc, float *pDst, int numData, const float *coeffs, int numSections, int stride);

/**
 * Fill the de-clatter structure used by de-clatter function
 * @param pstr pointer to de-clatter structure
 * @param tau_norm normalized time constsant (Tau over frame acquisition period, or Tau * framesPerSecond)
 * @return SUCCESS/FAIL
 */
int proc_compute_declatter_const(declattuer_constants_t * pstr, float tau_norm);


/**
 * Convert integer I/Q array into complex_float
 * @param pI I integer array
 * @param pQ Q integer array
 * @param pout complex_float output array
 * @param samples number of samples
 * @param scale scale factor applied to input integer array
 * @return SUCCESS/FAIL
 */
int proc_convert_int2complex_f (int32_t * pI, int32_t * pQ, complex_float* pout , int samples, float scale);

/**
 * Cross-correlation
 * @param pSrc input array
 * @param pRef reference signal (if array size is multiple of 4, correlation is executed faster)
 * @param pDest output array
 * @param sizeSrc input array size
 * @param sizeRef reference signal size
 * @param stride pSrc/pDest access stride (pRef is accessed sequentially)
 * @return SUCCESS/FAIL
 */

int proc_xcorr(const float *pSrc, const float *pRef, float *pDest, int sizeSrc, int sizeRef, int stride);



/**
 * @brief full stream processing
 *
 * This function executes\n
 * 1. Distortion correction (by providing alpha/nu values)\n
 * 2. BPF filtering\n
 * 3. De-clatter\n
 *
 * @param pstream stream handler
 * @param dist_alpha amplitude correction factor (set 1.0 if unused)
 * @param dist_nu phase correction factor (0.0 if unused)
 * @return SUCCESS/FAIL
 */
int proc_elaborate(proc_stream_t* pstream, float dist_alpha, float dist_nu);


#define PROC_SUCCESS 0
#define PROC_INVALID_PARAMS -1
#define PROC_FAIL -2

#endif /* PROCESSING_H_ */
