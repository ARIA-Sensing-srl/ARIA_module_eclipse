/*
 * reconstruction.h
 *
 *  Created on: Jan 16, 2024
 *      Author: ARIA Sensing
 */
#include "reconstruction_2d.h"
#include "reconstruction_types.h"
#include "reconstruction_aux_func.h"
#include "math_utils.h"
#include "stdlib.h"
#include "math.h"
#include "float.h"
#include "string.h"
#include "tbl_trig.h"
#include "cv32e40_core.h"

//** DEFINES START **//
#define TABLE_MASK 1023
#define TABLE_SIZE (TABLE_MASK+1)
#define OPT_ENABLE_SR_MODE 1
#define OPT_SR_ASM_VERSION 1	//replace section with assembler optimized one (-10% execution time)

//** DEFINES END **//

//** MACRO START **//

//** MACRO END **//

//** TYPES START **//
//** TYPES END **//

//** PRIVATE VARIABLES START **//
static float table_sin[TABLE_SIZE];
static float table_cos[TABLE_SIZE];
static uint8_t isinit = 0;
//** PRIVATE VARIABLES END **//

//** PUBLIC VARIABLES START **//

//** PUBLIC VARIABLES END **//

//** PRIVATE FUNCTIONS START **//
static inline complex_float priv_mult_cplx(complex_float a, complex_float b);
static inline float builtin_sqrt(float num);
static inline complex_float priv_tbl_exp(float norm_phase);
static void priv_tbl_init();
static int priv_DAS(recon_handler_t*ph ,  canvas_t* pcanvas, recon_stream_t * pstream, int numStreams, float normcarrier);
static int priv_DMAS(recon_handler_t*ph ,  canvas_t* pcanvas, recon_stream_t * pstream, int numStreams, float normcarrier);
static inline float priv_cplxabs(complex_float);
static inline float priv_cplxabssq(complex_float val);
static inline complex_float priv_cplxadd(complex_float val1, complex_float val2 );
static inline void store_cplxf32tocplxf16(float16_t * pdst, float re, float im);
static inline complex_float load_cplxf16tocplx32(float16_t * p);
//** PRIVATE FUNCTIONS END **//

//** PUBLIC FUNCTIONS START **//

//** PUBLIC FUNCTIONS END **//



int rec2D_init(recon_handler_t* ph, recalgs_t algo){
	priv_tbl_init();
	memset((void*)ph, 0, sizeof(recon_handler_t));
	ph->algo = algo; //set algorithm
	return REC2D_SUCCESS;
}


int rec2D_set_rho(recon_handler_t*p, float min, float max, float step, float fs){
	//compute base scale in samples for reconstruction
	uint32_t arraySize;
	arraySize = (uint32_t)ceilf((max-min)/step);
	if (p->pRhoBin){
		free((void*)p->pRhoBin);
		p->pRhoBin = NULL;
		p->rho_step = 0;
	}
	//allocate memory
	p->pRhoBin = (float*)malloc(arraySize * sizeof(float));
	if (p->pRhoBin == NULL)
		return REC2D_ERR_ALLOC;
	p->rho_step = step;
	p->rho_range[0] = min;
	p->rho_range[1] = max;
	p->rho_samples = arraySize;
	float binstep = LIGHT_SPEED/(fs*2.0);
	for(int i = 0 ; i< arraySize; i++){
		float curDist = min + (float)i*step; //calculate distance
		p->pRhoBin[i] = curDist/binstep; //convert into bins
	}
	return REC2D_SUCCESS;
}

static inline float builtin_sqrt(float num){
	float ret;
	asm volatile("fsqrt.s %0, %1" : "=f"(ret): "f"(num));
	return ret;
}

int rec2D_set_phi(recon_handler_t* p, float min, float max, float step, float fs){
	uint32_t arraySize;
	arraySize = (uint32_t)ceilf((max-min)/step);
	if (p->pDerivBin){
		free((void*)p->pDerivBin);
		p->pDerivBin = NULL;
		p->phi_step = 0;
	}
	//allocate memory
	p->pDerivBin = (float*)malloc(arraySize * sizeof(float));
	if (p->pDerivBin == NULL)
		return REC2D_ERR_ALLOC;
	p->phi_step = step;
	p->phi_range[0] = min;
	p->phi_range[1] = max;
	p->phi_samples = arraySize;
	for(int i = 0 ; i< arraySize;i++){
		float curPhi = min+(float)i*step;// - M_PI_2;
		float curTg = tanf(curPhi);
		float curder = curTg / builtin_sqrt(1+ curTg*curTg);
		p->pDerivBin[i]=curder*(2.0*fs)/LIGHT_SPEED; //embed constant, convert antenna dx meter in bins
	}
	return REC2D_SUCCESS;
}
canvas_t * rec2D_init_canvas(recon_handler_t*p){
	if (!(p->phi_samples) || !(p->rho_samples))
		return NULL;
	int canvasSize = p->phi_samples * p->rho_samples;
	canvas_t * pret = (canvas_t * )malloc(canvasSize * sizeof(canvas_t));
	return pret;
}

int rec2D_destroy_canvas(canvas_t** p){
	if (*p != NULL){
		free((void*)(*p));
		*p = NULL;
	}
	return REC2D_SUCCESS;
}

int rec2D_setup(recon_handler_t* p, int numStreams){
	if (!(p->phi_samples) || !(p->rho_samples))
		return REC2D_FAIL; //rho and phi are not allocated
	if (p->pctx){
		free((void*)p->pctx);
		p->pctx = NULL;
	}
	//check algo type
	if (p->algo == RA_DMAS){
		//needed extra memory to store partial results
		int canvasSize = p->phi_samples * p->rho_samples;
		p->pctx = malloc(2*sizeof(float16_t)*(numStreams-1)*canvasSize);
		if (p->pctx == NULL)
			return REC2D_FAIL;
	}
	return REC2D_SUCCESS;
}

int rec2D_setAlgo(recon_handler_t* p, recalgs_t newAlgo){
	if (p->algo != newAlgo){
		p->algo = newAlgo;
	}
	return 0;
}


/*
 * Reconstruction (currently based on DAS algorithm)
 * */
int rec2D_reconstruct(recon_handler_t*ph ,  canvas_t* pcanvas, recon_stream_t * pstream, int numStreams, float normcarrier){

	if (ph->algo == RA_DAS){
		return priv_DAS(ph, pcanvas,pstream, numStreams, normcarrier);
	}
	return priv_DMAS(ph, pcanvas,pstream, numStreams, normcarrier);

}


/*
 * Convert canvas from cplx float 16 to abs float32, pin and pout could be the same
 *
 * */
int rec2D_complex2abs(recon_handler_t*p ,  canvas_t* pin, canvas_t *pout){
	if (p->outputcplx == false)
		return REC2D_SUCCESS;

	int samples = p->phi_samples * p->rho_samples;
	for(int i = 0 ; i< samples; i++){
		complex_float curdata = load_cplxf16tocplx32((float16_t*)(&(pin[i])));
		float output = builtin_sqrt(curdata.imag*curdata.imag+ curdata.real*curdata.real);
		pout[i] = output;
	}
	return REC2D_SUCCESS;
}

/*
 * Dryrun, this routine emulates the 2D algorithm and check if indexes are out of bound.
 * The main reconstruction function doesn't check the OOB condition
 * */
int rec2D_reconstruct_dryrun(recon_handler_t*ph , recon_stream_t * pstream, int numStreams, int *lower_oob, int *upper_oob){

	int numPhi;
	int numRho;
	int ret = 0 ;

	*lower_oob = 0;
	*upper_oob = 0;

	uint32_t tfrm_csr = hal_spr_read(CSR_FRM);
	hal_spr_write(CSR_FRM, 2); //set round down

	numPhi = ph->phi_samples;
	numRho = ph->rho_samples;
	for (int phi = 0; phi < numPhi; phi ++){
		float curdelta = ph->pDerivBin[phi];
		for(int rho = 0 ; rho < numRho; rho++){
			float currho = ph->pRhoBin[rho];
			for (int s = 0 ; s < numStreams; s++){
				recon_stream_t * curstream = &pstream[s];
				float bin = currho + curdelta * (curstream->antennas_dx) - curstream->offset;
				int32_t bini;// = floorf(bin);
				asm volatile("fcvt.w.s %0, %1" : "=r"(bini) : "f"(bin));
				int32_t binn = bini+1;
				if (bini < 0){
					ret = REC2D_DRYRUN_OOB;
					if (bini < *lower_oob){
						*lower_oob = bini;
					}
				}
				if (binn >= curstream->size){
					ret = REC2D_DRYRUN_OOB;
					if (binn > *upper_oob ){
						*upper_oob = binn;
					}
				}
			}
		}
	}
	hal_spr_write(CSR_FRM, tfrm_csr); //restore
	return ret;
}


static inline complex_float priv_mult_cplx(complex_float a, complex_float b){
	complex_float ret;
	ret.real = a.real*b.real - a.imag*b.imag;
	ret.imag = a.real*b.imag + a.imag*b.real;
	return ret;
}


static inline complex_float priv_tbl_exp(float norm_phase){
	uint32_t index = (uint32_t)(norm_phase * TABLE_SIZE) & TABLE_MASK;
	complex_float ret;
	ret.imag = table_sin[index];
	ret.real = table_cos[index];
	return ret;
}
static void priv_tbl_init(){
	if (isinit)
		return;
	for (int i = 0 ; i< TABLE_SIZE; i++){
		float angle = M_PI*2.0*((float)i/(float)TABLE_SIZE);
		table_sin[i]=sinf(angle);
		table_cos[i]=cosf(angle);
	}
	isinit = 1;
}


//__attribute__((optimize("Ofast")))
static int priv_DAS(recon_handler_t*ph ,  canvas_t* pcanvas, recon_stream_t * pstream, int numStreams, float normcarrier){
	//asm volatile(".option norvc");
	int numPhi;
	int numRho;
	numPhi = ph->phi_samples;
	numRho = ph->rho_samples;
	float normFactor = 1/(float)numStreams;

	uint32_t tfrm_csr = hal_spr_read(CSR_FRM);
	hal_spr_write(CSR_FRM, 2); //set round down mode

	for (int phi = 0; phi < numPhi; phi ++){
		register float curdelta = ph->pDerivBin[phi];
		register complex_float pixel;
		canvas_t * pRho = pcanvas + phi*numRho;
		for(int rho = 0 ; rho < numRho; rho++){
			float currho = ph->pRhoBin[rho];
			pixel.real = pixel.imag = 0;
			for (int s = 0 ; s < numStreams; s++){
				recon_stream_t * curstream = &pstream[s];
				float bin = currho + curdelta * (curstream->antennas_dx ) - curstream->offset;
				uint32_t bini;// = floorf(bin);
				asm volatile("fcvt.w.s %0, %1" : "=r"(bini) : "f"(bin)); //with round down keep integer part
				//uint32_t binn = bini+1;
				float fract = bin-bini;
				complex_float a,b;
				complex_float twiddle;
				twiddle = priv_tbl_exp(bin*normcarrier);
				a = curstream->pdata[bini];
				b = curstream->pdata[bini+1];
				//DAS
				a.real += (b.real-a.real)* fract;// + a.real;
				a.imag += (b.imag-a.imag)* fract;// + a.imag;
				b = priv_mult_cplx(a, twiddle);
				pixel.real += b.real;
				pixel.imag += b.imag;
			}
			canvas_t pixelOut = builtin_sqrt(pixel.real*pixel.real+pixel.imag*pixel.imag);
			/*pixel.real = fabs(pixel.real);
			pixel.imag = fabs(pixel.imag);
			if (pixel.real > pixel.imag){
				pixelOut = pixel.real*0.96043387 + pixel.imag*0.39782473;
			}else{
				pixelOut = pixel.imag*0.96043387 + pixel.real*0.39782473;
			}*/
			if (ph->outputcplx == false){
				//store absolute value
				pRho[rho] = pixelOut*normFactor;
			} else {
				//scale
				pixel.real *= normFactor;
				pixel.imag *= normFactor;
				store_cplxf32tocplxf16((float16_t*)(&pRho[rho]), pixel.real, pixel.imag);
			}
		}
	}
	hal_spr_write(CSR_FRM, tfrm_csr); //restore ronding mode
	return REC2D_SUCCESS;
}


static inline void store_cplxf32tocplxf16(float16_t * pdst, float re, float im){
	float converted;
	int storage;
	asm volatile (
		"fcvt.h.s %[i], %[j];"
		"fmv.x.h %[h], %[i];"
		"p.sh %[h], 2(%[t]!)"
		: [i] "=f" (converted), [j] "+f" (re), [h] "=r" (storage), [t] "+r" (pdst)
	);
	asm volatile (
		"fcvt.h.s %[i], %[j];"
		"fmv.x.h %[h], %[i];"
		"p.sh %[h], 2(%[t]!)"
		: [i] "=f" (converted), [j] "+f" (im), [h] "=r" (storage), [t] "+r" (pdst)
	);

	/*uint32_t st, tmp;
	float c1, c2;

	asm volatile(
			"fcvt.h.s %[c1], %[re];"
			"fcvt.h.s %[c2], %[im];"
			"fmv.x.h %[st], %[c1];"
			"fmv.x.h %[tmp], %[c2];"
			"pv.pack %[st], %[tmp], %[st];"
			"sw %[st], 0(%[d]);"
			: [c1] "+f" (c1), [c2] "+f" (c2), [re] "+f" (re), [im] "+f" (im), [st] "+r" (st), [tmp] "+r" (tmp), [d] "+r" (pdst)
	);*/ //implementation with extractu is slower


}

static inline complex_float load_cplxf16tocplx32(float16_t * p){
	complex_float retcmpl;
	float re, im;
	float converted;
	int storage;
	asm volatile (
		"p.lh %[h], 2(%[t]!);"
		"fmv.h.x %[i], %[h];"
		"fcvt.s.h %[j], %[i]"
		: [i] "=f" (converted), [j] "=f" (re), [h] "=r" (storage), [t] "+r" (p)
	);
	asm volatile (
		"p.lh %[h], 2(%[t]!);"
		"fmv.h.x %[i], %[h];"
		"fcvt.s.h %[j], %[i]"
		: [i] "=f" (converted), [j] "=f" (im), [h] "=r" (storage), [t] "+r" (p)
	);

	/*uint32_t t0, t1;
	float re, im;
	asm volatile (
			"lw %[t0], 0(%[p]);"
			"pv.extractu.h %[t1], %[t0], 0;"
			"fmv.h.x %[re], %[t1];"
			"fcvt.s.h %[re], %[re];"
			"pv.extractu.h %[t1], %[t0], 1;"
			"fmv.h.x %[im], %[t1];"
			"fcvt.s.h %[im], %[im];"
			: [re] "=f" (re), [im] "=f" (im), [t0] "+r" (t0), [t1] "+r" (t1), [p] "+r" (p)
	);*/ //implementation with extractu is slower

	retcmpl.real = re;
	retcmpl.imag = im;
	return retcmpl;
}

static inline float priv_cplxabs(complex_float val){
	return builtin_sqrt(val.real * val.real + val.imag*val.imag);
}

static inline float priv_cplxabssq(complex_float val){
	return (val.real * val.real + val.imag*val.imag);
}

static inline complex_float priv_cplxadd(complex_float val1, complex_float val2 ){
	complex_float ret;
	ret.real = val1.real+val2.real;
	ret.imag = val1.imag+val2.imag;
	return ret;
}

//__attribute__((optimize("Ofast")))
static int priv_DMAS(recon_handler_t*ph ,  canvas_t* pcanvas, recon_stream_t * pstream, int numStreams, float normcarrier){
	int numPhi;
	int numRho;
	float normFactor;
	{
		int temp = 0;
		for(int i = 1 ; i< numStreams; i++){
			temp += i;
		}
		normFactor = 1/(float)temp;
	}


	uint32_t tfrm_csr = hal_spr_read(CSR_FRM);
	hal_spr_write(CSR_FRM, 2); //set round down mode
	numPhi = ph->phi_samples;
	numRho = ph->rho_samples;
	int canvasPoints = numPhi * numRho;
	for (int phi = 0; phi < numPhi; phi ++){
		float curdelta = ph->pDerivBin[phi];
		complex_float pixel;
		canvas_t * pRho = pcanvas + phi*numRho;
		float * pPartial = ((float *)ph->pctx) + phi*numRho; //partial store I/Q could be handled as float
		for(int rho = 0 ; rho < numRho; rho++){
			float currho = ph->pRhoBin[rho];

			//pixel.real = pixel.imag = 0;
			for (int s = 0 ; s < numStreams; s++){
				recon_stream_t * curstream = &pstream[s];
				complex_float twiddle;
				float bin = currho + curdelta * (curstream->antennas_dx );// - curstream->offset;
				bin = bin - curstream->offset; //mind the order (do not switch) with tbl_exp
				twiddle = priv_tbl_exp(bin*normcarrier);


				uint32_t bini;// = floorf(bin);
				asm volatile("fcvt.w.s %0, %1" : "=r"(bini) : "f"(bin)); //with round down keep integer part
				//uint32_t binn = bini+1;
				float fract = bin-bini;
				complex_float a,b;
				a = curstream->pdata[bini];
				b = curstream->pdata[bini+1];
				a.real += (b.real-a.real)* fract + a.real;
				a.imag += (b.imag-a.imag)* fract + a.imag;
				pixel = priv_mult_cplx(a, twiddle);
				if (s == 0){
					store_cplxf32tocplxf16((float16_t*)(&pRho[rho]), pixel.real, pixel.imag);
				}else{
					store_cplxf32tocplxf16((float16_t*)(&pPartial[rho + (s-1)*(canvasPoints)]), pixel.real, pixel.imag);
				}
			}
		}
	}
	hal_spr_write(CSR_FRM, tfrm_csr); //restore rounding mode
	//span canvas and compute correlation
	float * pPartial = ((float *)ph->pctx);
	for (int i = 0; i< canvasPoints; i++){
		complex_float pixelc;
#if OPT_ENABLE_SR_MODE == 1
		complex_float cfCplxCumSum;
		float cfCumAbsSum;
#endif
		complex_float a = load_cplxf16tocplx32((float16_t*) &pcanvas[i]);
#if (OPT_SR_ASM_VERSION && OPT_ENABLE_SR_MODE) == 1
		cfCplxCumSum = a;
		cfCumAbsSum = priv_cplxabssq(a);
		pixelc = rec_partCFcompute_asm((float16_t*)&pPartial[i],canvasPoints, numStreams, &cfCplxCumSum, &cfCumAbsSum);
#else
		pixelc.real = 0;
		pixelc.imag = 0;
#if OPT_ENABLE_SR_MODE == 1
		cfCplxCumSum = a;
		cfCumAbsSum = priv_cplxabssq(a);
#endif
		for(int s = 0; s < numStreams-1; s++){
			for(int t = (s+1) ; t < numStreams; t++){
				complex_float b = load_cplxf16tocplx32((float16_t*) &pPartial[i+(t-1)*canvasPoints]);
				b = priv_mult_cplx(b,a);
				pixelc.real += b.real;
				pixelc.imag += b.imag;
			}
			a = load_cplxf16tocplx32((float16_t*) &pPartial[i+s*canvasPoints]);
#if OPT_ENABLE_SR_MODE == 1
			cfCplxCumSum = priv_cplxadd(cfCplxCumSum, a);
			cfCumAbsSum += priv_cplxabssq(a);
#endif
		}
#endif
#if OPT_ENABLE_SR_MODE == 1
		float CF = priv_cplxabssq(cfCplxCumSum);
		CF = CF/cfCumAbsSum;
#endif
		if (!ph ->outputcplx){
			//store normalized value for the output image
			float pixelAmpl = builtin_sqrt((pixelc.real*pixelc.real + pixelc.imag*pixelc.imag)*normFactor);
			pixelAmpl *= CF;
			pcanvas[i] = pixelAmpl;
		}else{
			//store a complex f16
#if OPT_ENABLE_SR_MODE == 1
			pixelc.real *= CF;
			pixelc.imag *= CF;
#endif
			//store data as a complex f16
			store_cplxf32tocplxf16((float16_t*)(&pcanvas[i]), pixelc.real, pixelc.imag);
		}
	}
	return 0;
}
