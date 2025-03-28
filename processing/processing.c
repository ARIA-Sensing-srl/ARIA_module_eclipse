/*
 * processing.c
 *
 *  Created on: Jan 16, 2023
 *      Author: ARIA Sensing
 */
#include "processing.h"
#include "wavelet.h"
#include "fe_math.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"


//** DEFINES START **//
#define NUMEL(x) 			(sizeof(x)/sizeof(x[0]))
#define BIQ_CFS_SIZE 		NUMEL(biqcfs)//(sizeof(biqcfs)/sizeof(biqcfs[0]))
#define BIQ_SEC_NUM 		(BIQ_CFS_SIZE/5)
#define WVLT_H_NUM_COEFFS 	NUMEL(wvlt_h)
#define WVLT_G_NUM_COEFFS 	NUMEL(wvlt_g)
#define AUX0_ARRAY_SIZE		(512+16)	//
#define AUX1_ARRAY_SIZE		(512+16)
#define WAVELET_LEVEL 8
#define WAVELET_ZEROING_LIST 0,0,0,0,0,0,0,1,0
#define CORR_REF_SIZE		NUMEL(corr_ref)

//compiler elaboration options
#define OPT_CORRELATION_ENABLE 1
#define OPT_ENABLE_HPF 1

//** DEFINES END **//

//** MACRO START **//

//** MACRO END **//

//** TYPES START **//

//** TYPES END **//

//** PRIVATE VARIABLES START **//
/*//filter lowpass Elliptical, order 5, inband ripple 2dB, stopband attenation 60 dB, fcut_norm = 450/1536
static const float biqcfs[] = {	1.0, 	0.1145,		0.0694, 0.4754, 	0.9014, 	\
								1.0,	1.2849,		1.0,	-0.1769,	0.5952,		\
								1.0,	1.0,		0,		-0.5065,	0};*/

//filter highpass butter, order 7, fcut_norm = 50/1536
/*static const float biqcfs[] = { 0.63090,  -1.25593,   0.62507,   -1.65539,   0.69063, \
								1.00000,  -2.00309,   1.00315,   -1.73820,   0.77520, \
								1.00000,  -2.01349,   1.01355,   -1.87363,   0.91352, \
								1.00000,  -0.99271,   0.00000,   -0.81386,   0.00000};*/

//filter bandpass butter, order 6, [10 400]/1536
/*static const float biqcfs[] = {		0.031831,   0.063737,  0.031906,   0.100896,   0.601068, \
								   1.000000,   1.997659,   0.997664,   0.055209,   0.181930, \
								   1.000000,  -1.997586,   0.997592,   0.029143,   0.018973, \
								   1.000000,  -2.002407,   1.002413,  -1.919745,   0.921476, \
								   1.000000,   2.000007,   1.000002,  -1.942188,   0.943876, \
								   1.000000,  -2.000007,   1.000001,  -1.978075,   0.979739};*/

//filter bandpass butter, order 3, [20 700]/1792
/*static const float biqcfs[] = { 	0.459696,	0.91939,	0.459694,	1.17406,	0.523653, \
									1,			-1.99999,	0.999994,	-1.92835,	0.933157, \
									1,			3.81737e-08,-1.00001,	-0.467611,	-0.430742};*/

//filter bandpass butter, order 3, [20 500]/1792
static const float biqcfs[] = {	0.196146,	0.392291,	0.196145,	0.228723,	0.359741, \
								1,			-1.99999,	0.999994,	-1.92949,	0.934378, \
								1,			3.81737e-08,-1.00001,	-0.867519,	-0.0561588};

//correlation coefficentts
static const float corr_ref[] = {-0.0163979,-0.0202657,-0.0314327,-0.0503852,-0.0711345,-0.0727092,0.134961,0.492599,0.701928,0.434999,0.0530335,-0.12833,-0.0892166,-0.0785502,-0.0706852,-0.0464298,-0.0348388,-0.0235116,-0.0150105,-0.00987792};
static const int ref_delay = 8;

//wavelet filter coefficients
//sym4 mother wavelet
static const float wvlt_h []= 		{ -0.075766,  -0.029636,   0.497619,   0.803739 ,  0.297858,  -0.099220 , -0.012604 ,  0.032223 };
static const float wvlt_rev_h[] =	{0.032223 , -0.012604 , -0.099220 ,  0.297858 ,  0.803739   ,0.497619  ,-0.029636 ,-0.075766};
static const float wvlt_g []= { -0.032223, -0.012604,   0.099220 ,  0.297858 , -0.803739 ,  0.497619,  0.029636 , -0.075766 };
static const float wvlt_rev_g[] = {-0.075766 ,  0.029636 ,  0.497619 , -0.803739  , 0.297858 ,  0.099220 , -0.012604 , -0.032223};
static const int wvlt_lvl = WAVELET_LEVEL;
static int scale_info[WAVELET_LEVEL+1];
static const uint8_t zeroing_level_list[WAVELET_LEVEL+1] = {WAVELET_ZEROING_LIST};


//auxiliar arrays
union _aux0{
	struct {
		float f0[AUX0_ARRAY_SIZE];
		float f1[AUX0_ARRAY_SIZE];
	};
	complex_float cf[AUX0_ARRAY_SIZE];
}aux0;
union _aux1{
	float f[AUX1_ARRAY_SIZE];
}aux1;


//** PRIVATE VARIABLES END **//

//** PUBLIC VARIABLES START **//

//** PUBLIC VARIABLES END **//

//** PRIVATE FUNCTIONS START **//
static void priv_proc_declatter(const complex_float *pSrc, complex_float *pClatter, complex_float * pDest, int size, declattuer_constants_t * pconst, bool isclean);
static int priv_proc_wavelet_filtering(complex_float *pSrc, complex_float *pDst, int size);
static int priv_proc_dist_correction(complex_float *pSrc, float alpha, float nu, int size);
//** PRIVATE FUNCTIONS END **//

//** PUBLIC FUNCTIONS START **//

//** PUBLIC FUNCTIONS END **//



int proc_elaborate(proc_stream_t* p, float dist_alpha, float dist_nu){
	if (p->size > AUX0_ARRAY_SIZE)
		return PROC_FAIL;

	complex_float * praw = p->praw;
	complex_float * pclutter = p->pclutter;
	complex_float * pout = p->pout;


	//filtfilt
	priv_proc_dist_correction(praw, dist_alpha, dist_nu, p->size);
#if (OPT_ENABLE_HPF == 1)
	for(int i = 0 ; i< p->size; i++){
		pout[i] = praw[i];
	}
	proc_filtfilt_biquad(&(pout->real), &(pout->real), p->size, biqcfs, BIQ_SEC_NUM, 2);
	proc_filtfilt_biquad(&(pout->imag), &(pout->imag), p->size, biqcfs, BIQ_SEC_NUM, 2);
	priv_proc_declatter(pout, pclutter, pout, p->size, p->pdecl, p->clutter_empty);
#else

	priv_proc_declatter(praw, pclutter, pout, p->size, p->pdecl, p->clutter_empty);
#endif
	//wavelet filtering
	//priv_proc_wavelet_filtering(pout, pout, p->size);
	//frame is DC free proceed with clutter update and substraction
	p->clutter_empty = false;

	return PROC_SUCCESS;
}


void proc_filtfilt_biquad(const float *pSrc, float *pDst, int numData, const float *coeffs, int numSections, int stride){
	proc_iir_biquad(pSrc, pDst, numData, coeffs, numSections, stride, 0);
	proc_iir_biquad(pSrc, pDst, numData, coeffs, numSections, stride, 1);
}


void proc_iir_biquad_C(const float *pSrc, float *pDst, int numData,
		const float *coeffs, int numSections, int stride, int reverse) {
	//coeffs order b0, b1, b2, a1, a2
	if (!numSections)
		return;
	float *pauxSrc, *pauxDst;


	int s = 0;

	if (reverse){
		pSrc = (pSrc+numData*stride)-stride;
		pDst = (pDst+numData*stride)-stride;
		stride = -stride;
	}

	pauxSrc = (float*)pSrc;
	pauxDst = pDst;

	if (numSections >= 2) {
		//compute two section per step
		int numSecFast;
		numSecFast = numSections & (~1);
		for (; s < numSecFast; s += 2) {
			float a01, a02, b00, b01, b02;
			float z0p, z0pp;
			float a11, a12, b10, b11, b12;
			float z1p, z1pp;
			float acc, acc2;
			z0p = z0pp = 0;
			z1p = z1pp = 0;
			b00 = coeffs[5 * s];
			b01 = coeffs[5 * s + 1];
			b02 = coeffs[5 * s + 2];
			a01 = coeffs[5 * s + 3];
			a02 = coeffs[5 * s + 4];

			b10 = coeffs[5 * s + 5];
			b11 = coeffs[5 * s + 6];
			b12 = coeffs[5 * s + 7];
			a11 = coeffs[5 * s + 8];
			a12 = coeffs[5 * s + 9];

			for (int i = 0; i < numData; i++) {
				acc = *pauxSrc;
				pauxSrc += stride;
				//section0
				acc -= z0p * a01;
				acc -= z0pp * a02;
				acc2 = acc * b00;
				acc2 += z0p * b01;
				acc2 += z0pp * b02;
				z0pp = z0p;
				z0p = acc;
				//section 1
				acc2 -= z1p * a11;
				acc2 -= z1pp * a12;
				acc = acc2 * b10;
				acc += z1p * b11;
				acc += z1pp * b12;
				z1pp = z1p;
				z1p = acc2;

				*pauxDst = acc;
				pauxDst += stride;
			}
			pauxDst = pDst;
			pauxSrc = pDst;	//second section executed insitu
		}
	}

	for (; s < numSections; s++) {
		//load coeffs
		float a1, a2, b0, b1, b2;
		float zp, zpp;
		float acc, acc2;
		zp = zpp = 0;
		b0 = coeffs[5 * s];
		b1 = coeffs[5 * s + 1];
		b2 = coeffs[5 * s + 2];
		a1 = coeffs[5 * s + 3];
		a2 = coeffs[5 * s + 4];
		for (int i = 0; i < numData; i++) {
			//acc = pBuf[i];
			acc = *pauxSrc;
			pauxSrc += stride;

			acc -= zp * a1;
			acc -= zpp * a2;
			acc2 = acc * b0;
			acc2 += zp * b1;
			acc2 += zpp * b2;
			//pDst[i] = acc2;
			*pauxDst = acc2;
			pauxDst += stride;
			zpp = zp;
			zp = acc;
		}
		pauxDst = pDst;
		pauxSrc = pDst;	//second section executed insitu
	}
}

//processing declatter
static void priv_proc_declatter(const complex_float *pSrc, complex_float *pClatter, complex_float * pDest, int size, declattuer_constants_t * pconst, bool isclean){
	float alpha, beta;
	alpha = pconst->alpha;
	beta = pconst->beta;

	if (!isclean){
		//update clutter matrix
		for(int i = 0 ; i < size; i++){
			float curRe, curIm;
			float clutterRe, clutterIm;
			curRe = pSrc[i].real;
			curIm = pSrc[i].imag;
			clutterRe = pClatter[i].real;
			clutterIm = pClatter[i].imag;
			clutterRe = clutterRe*alpha + curRe*beta;
			clutterIm = clutterIm*alpha + curIm*beta;
			pClatter[i].real = clutterRe;
			pClatter[i].imag = clutterIm;
			pDest[i].real = curRe - clutterRe;
			pDest[i].imag = curIm - clutterIm;
		};
	} else {
		//clutter matrix is free, save data and return 0
		for(int i = 0 ; i < size; i++){
			float curRe, curIm;
			curRe = pSrc[i].real;
			curIm = pSrc[i].imag;
			pClatter[i].real = curRe;
			pClatter[i].imag = curIm;
			pDest[i].real = 0;
			pDest[i].imag = 0;
		}
	}
}


static int priv_proc_dist_correction(complex_float *pSrc, float alpha, float nu, int size){
	float A,B,C;
	A = 1/alpha;
	B = tanf(nu)*A;
	C = 1/cosf(nu);
	for(int i =  0; i < size; i++){
		complex_float cur;
		complex_float corr;
		cur = pSrc[i];
		corr.real = cur.real * A;
		corr.imag = -cur.real * B + cur.imag*C;
		pSrc[i] = corr;
	}
	return 0;
}


/*
 * Allowed in-situ processing
 * */
static int priv_proc_wavelet_filtering(complex_float *pSrc, complex_float *pDst, int size){
	//check size
	int wvltArraySize = wlt_calc_min_size(size, wvlt_lvl); //min wavelet array size, total size include filter header and trailer
	//check size
	if (wvltArraySize+2*WVLT_H_NUM_COEFFS > AUX1_ARRAY_SIZE){
		//copy elements to destination and return error;
		memcpy((void*)pDst, (const void *)pSrc, sizeof(complex_float)* size);
		return -1;
	}
	//wavelet fit arrays, proceed
	wlt_calc_scale_info(scale_info, wvlt_lvl, wvltArraySize);

	//transfer data into aux0
	{
		int i;
		for (i = 0 ; i< size ; i++){
			aux0.f0[i]=pSrc[i].real;
		}
		for (; i < wvltArraySize; i++){
			aux0.f0[i]=0;
		}
	}
	//execute wavelet transform
	wlt_fwt(aux0.f0, aux0.f0, wvltArraySize, aux1.f, wvlt_rev_h, wvlt_rev_g, WVLT_G_NUM_COEFFS, wvlt_lvl);
	float *p;
	p = aux0.f0;
	for (int n = 0 ; n< (wvlt_lvl+1); n++){
		if (zeroing_level_list[n] == 0){
			p += scale_info[n];
		}else{
			for (int i = 0 ; i< scale_info[n]; i++){
				*p++ = 0.0;
			}
		}
	}
	//inverse
	wlt_ifwt(aux0.f0, aux0.f1, wvltArraySize, aux1.f, wvlt_h, wvlt_g, WVLT_G_NUM_COEFFS, wvlt_lvl);
	for(int i = 0 ; i< size; i++){
		pDst[i].real = aux0.f1[i];
	}

	//transfer data into aux0
	{
		int i;
		for (i = 0 ; i< size ; i++){
			aux0.f0[i]=pSrc[i].imag;
		}
		for (; i < wvltArraySize; i++){
			aux0.f0[i]=0;
		}
	}
	//execute wavelet transform
	wlt_fwt(aux0.f0, aux0.f0, wvltArraySize, aux1.f, wvlt_rev_h, wvlt_rev_g, WVLT_G_NUM_COEFFS, wvlt_lvl);
	p = aux0.f0;
	for (int n = 0 ; n< (wvlt_lvl+1); n++){
		if (zeroing_level_list[n] == 0){
			p += scale_info[n];
		}else{
			for (int i = 0 ; i< scale_info[n]; i++){
				*p++ = 0.0;
			}
		}
	}
	//inverse
	wlt_ifwt(aux0.f0, aux0.f1, wvltArraySize, aux1.f, wvlt_h, wvlt_g, WVLT_G_NUM_COEFFS, wvlt_lvl);
	for(int i = 0 ; i< size; i++){
		pDst[i].imag = aux0.f1[i];
	}
	return 0;
}

__attribute__ ((optimize("-Os")))
int proc_compute_declatter_const(declattuer_constants_t * pstr, float tau_norm){
	if (tau_norm >= 0.0){
		pstr->alpha = tau_norm/(1.0+tau_norm);
		pstr->beta = 1-pstr->alpha;
	}else{
		tau_norm = -1; //this setting disable the declutter, image are reconstructed with static data
		pstr->alpha = 0;
		pstr->beta = 0;
	}
	pstr->normTau = tau_norm; //store normalized
	return PROC_SUCCESS;
}


int proc_convert_int2complex_f(int32_t * pI, int32_t * pQ, complex_float* pcplx, int samples, float scale){
	for (int i = 0 ; i< samples; i++){
		pcplx[i].real = ((float)pI[i]) * scale;
		pcplx[i].imag = ((float)pQ[i]) * scale;
	}
	return PROC_SUCCESS;
}

int proc_xcorr(const float *pSrc, const float *pRef, float *pDest, int sizeSrc, int sizeRef, int stride) {
	int it, orgIt;
	if (!sizeSrc || !sizeRef)
		return -1;

	orgIt = sizeSrc - sizeRef;
	it = sizeSrc - sizeRef;
	int i;
	float Acc = 0;
	if (sizeRef & 3) {
		//not a multiple of 4 execute normal
		for (i = 0; i <= it; i++) {
			Acc = 0;
			for (int k = 0; k < sizeRef; k++) {
				Acc += pSrc[(i + k)*stride] * pRef[k];
			}
			pDest[i*stride] = Acc;
		}
	} else {
		//execute fast
		it -= 4;
		it &= ~3;

		for (i = 0; i < it; i += 4) {
			Acc = 0;
			float Acc1 = 0;
			float Acc2 = 0;
			float Acc3 = 0;

			float x0, x1, x2, x3;
			float c;
			x0 = pSrc[(i)*stride];
			x1 = pSrc[(i + 1)*stride];
			x2 = pSrc[(i + 2)*stride];

			for (int k = 0; k < sizeRef; k += 4) {
				//c0
				c = pRef[k];
				x3 = pSrc[(i + k + 3)*stride];
				Acc += x0 * c;
				Acc1 += x1 * c;
				Acc2 += x2 * c;
				Acc3 += x3 * c;
				//c1, replace x0
				c = pRef[k + 1];
				x0 = pSrc[(i + k + 4)*stride];
				Acc += x1 * c;
				Acc1 += x2 * c;
				Acc2 += x3 * c;
				Acc3 += x0 * c;
				//c2, replace x1
				c = pRef[k + 2];
				x1 = pSrc[(i + k + 5)*stride];
				Acc += x2 * c;
				Acc1 += x3 * c;
				Acc2 += x0 * c;
				Acc3 += x1 * c;
				//c3, replace x2
				c = pRef[k + 3];
				x2 = pSrc[(i + k + 6)*stride];
				Acc += x3 * c;
				Acc1 += x0 * c;
				Acc2 += x1 * c;
				Acc3 += x2 * c;
			}
			pDest[(i)*stride] = Acc;
			pDest[(i + 1)*stride] = Acc1;
			pDest[(i + 2)*stride] = Acc2;
			pDest[(i + 3)*stride] = Acc3;
		}
	}

	//residual
	for (i; i <= orgIt; i++) {
		Acc = 0;
		for (int k = 0; k < sizeRef; k++) {
			Acc += pSrc[(i + k)*stride] * pRef[k];
		}
		pDest[(i)*stride] = Acc;
	}

	for (; i < sizeSrc; i++) {
		Acc = 0;
		for (int k = i; k < sizeSrc; k++) {
			Acc += pSrc[(k)*stride] * pRef[k - i];
		}
		pDest[(i)*stride] = Acc;
	}

	return 0;
}
