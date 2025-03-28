/**
 * @file reconstruction.h
 * @brief Routines for 2D image reconstruction
 *
 * Implemented two different reconstruction algorithm\n
 * DAS: delay-and-sum, fast algorithm and few memory required, large side-lobe\n
 * DMAS: delay-mutiply-and-sum, complex computing and large memory required, good image reconstruction performance\n
 * The output image is mapped as range/azimuth (spherical coordinates) and the reconstrucion area is set in the same way
 * Part of the internal routines are implemented in assembler and are designed to exploit some core features\n
 *
 *  Created on: Jan 16, 2024\n
 *  Author: ARIA Sensing\n
 */

#ifndef RECONSTRUCTION_2D_H_
#define RECONSTRUCTION_2D_H_


#include "fe_math.h"
#include "reconstruction_types.h"
#include "stdbool.h"

/**
 * Reconstruction handler
 */
typedef struct _recon_handler_t{
	float rho_range[2];
	float rho_step;
	float phi_range[2];
	float phi_step;
	uint32_t rho_samples;
	uint32_t phi_samples;
	float * pRhoBin;
	float * pDerivBin;
	void * pctx;
	recalgs_t algo;
	bool outputcplx;
}recon_handler_t;

/**
 * Setup a new reconstruction handler
 * @param h
 * @param algo selected image reconstruction algorithm
 * @return SUCCESS/FAIL
 */
int rec2D_init(recon_handler_t*h, recalgs_t algo);


/**
 * Set the image downrange reconstruction boundaries
 * @param h
 * @param min minium range in meters
 * @param max maximum range in meters
 * @param step resolution in meters
 * @param fs ADC sample frequency Hz
 * @return SUCCESS/FAIL
 */
int rec2D_set_rho(recon_handler_t*h, float min, float max, float step, float fs);

/**
 * Set the image crossrange reconstruction boundaries
 * @param h
 * @param min minimum azimuth in rad
 * @param max maximum azimuth in rad
 * @param step resolution in rad
 * @param fs ADC sample frequency Hz
 * @return SUCCESS/FAIL
 */
int rec2D_set_phi(recon_handler_t*h, float min, float max, float step, float fs);


/**
 * Setup auxiliary variables needed for reconstruction
 * @param h
 * @param numStreams
 * @return SUCCESS/FAIL
 */
int rec2D_setup(recon_handler_t*h, int numStreams);

/**
 * Allocate required memory for the output image
 * @param h
 * @return pointer to output image
 */
canvas_t * rec2D_init_canvas(recon_handler_t*h);

/**
 * Deallocate image memory
 * @param h
 * @return SUCCESS/FAIL
 */
int rec2D_destroy_canvas(canvas_t** h);

/**
 * Calculate the output image according to the reconstruction handler parameters
 * @param p reconstruction handler
 * @param canvas image memory area
 * @param pstream pointer to input streams array
 * @param streams number of streams
 * @param normcarrier normalized carrier (fcarrier/fADC)
 * @return SUCCESS/FAIL
 */
int rec2D_reconstruct(recon_handler_t*p ,  canvas_t* canvas , recon_stream_t * pstream, int streams, float normcarrier);

/**
 * Convert output image from complex float16_t to absolute value float32\n
 * Convertion could be executed in-place (pin==pout)
 * @param p
 * @param pin	input image
 * @param pout  output image
 * @return SUCCESS/FAIL
 */
int rec2D_complex2abs(recon_handler_t*p ,  canvas_t* pin, canvas_t *pout);


/**
 * @brief This function emulate the reconstruction algorithm without create an actual image\n
 * This function must be called after canvas/stream range set and before try an actual image reconstruction\n
 * In order to speedup the image reconstruction, the actual algorithm doesn't check if the stream indexes (calculated for each image pixel) is inside the stream's boundaries\n
 * Hence the user must ensure that image boundaries and stream length must be properly set\n
 * \n
 * The function calculates,for each image pixel, the index of sample for each stream, check if the index is inside the stream's boundaries and return the out-of-bound index\n
 * \n
 * If the OOB indexes are different from zero, means that there are some image's pixel that could not be evaluated with the current acquisition stream setting\n
 * Users must trim canvas area or acquisition range to allow the correct computation of each pixel
 *
 * @param ph
 * @param pstream
 * @param numStreams
 * @param lower_oob
 * @param upper_oob
 * @return SUCCESS or DRYRUN_OOB (detected out-of-bound stream access)
 */
int rec2D_reconstruct_dryrun(recon_handler_t*ph , recon_stream_t * pstream, int numStreams, int *lower_oob, int *upper_oob);


/**
 * Change the reconstruction algorithm
 * @param p
 * @param newAlgo
 * @return  SUCCESS/FAIL
 */
int rec2D_setAlgo(recon_handler_t* p, recalgs_t newAlgo);

#define REC2D_SUCCESS 0
#define REC2D_FAIL -1
#define REC2D_ERR_ALLOC -2
#define REC2D_DRYRUN_OOB -3

#endif /* RECONSTRUCTION_2D_H_ */
