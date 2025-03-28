/**
 * @file reconstruction_3d
 * @brief Routines for 3D volume reconstruction
 *
 * Implemented two different reconstruction algorithm\n
 * DAS: delay-and-sum, fast algorithm and few memory required, large side-lobe\n
 * DMAS: delay-mutiply-and-sum, complex computing and large memory required, good image reconstruction performance\n
 * 3D volume reconstruction requires a large amount of memory and only DAS work without problem\n
 * The output volume is mapped as range/azimuth/elevation (spherical coordinates) and the reconstrucion area is set in the same way
 * Part of the internal routines are implemented in assembler and are designed to exploit some core features\n
 *
 *  Created on: Jan 16, 2024\n
 *  Author: ARIA Sensing\n
 */

#ifndef RECONSTRUCTION_3D_H_
#define RECONSTRUCTION_3D_H_

#include "fe_math.h"
#include "reconstruction_types.h"

/**
 * Reconstruction handler
 */
typedef struct _recon3d_handler_t{
	float rho_range[2];
	float rho_step;
	float el_range[2];		//elevation
	float el_step;
	float theta_range[2];	//azimuthal
	float theta_step;
	uint32_t rho_samples;
	uint32_t el_samples;
	uint32_t theta_samples;
	float * pRhoBin;
	float * pDerivBin; //2D plane
	void * pctx;
	recalgs_t algo;
}recon3d_handler_t;

/**
 * Setup a new reconstruction handler
 * @param h
 * @param algo selected image reconstruction algorithm
 * @return SUCCESS/FAIL
 */
int rec3D_init(recon3d_handler_t*, recalgs_t);

/**
 * Set the volume downrange reconstruction boundaries
 * @param h
 * @param min minium range in meters
 * @param max maximum range in meters
 * @param step resolution in meters
 * @param fs ADC sample frequency Hz
 * @return SUCCESS/FAIL
 */
int rec3D_set_rho(recon3d_handler_t*h, float min, float max, float step, float fs);

/**
 * Set the volume cross-range boundaries
 * @param h
 * @param elmin minimum elevation in rad
 * @param elmax maximum elevation in rad
 * @param elstep elevation resolution in rad
 * @param thetamin minimum azimuth in rad
 * @param thetamax maximum azimuth in rad
 * @param thetastep azimuth resolution in rad
 * @param fs carrier carrier frequency in Hz
 * @return SUCCESS/FAIL
 */
int rec3D_set_angles(recon3d_handler_t*h, float elmin, float elmax, float elstep, float thetamin, float thetamax, float thetastep, float fs);


/**
 * Setup auxiliary variables needed for reconstruction
 * @param h
 * @param numStreams
 * @return SUCCESS/FAIL
 */
int rec3D_setup(recon3d_handler_t*h, int numStreams);

/**
 * Auxiliary function for memory management
 * @param p
 * @return SUCCESS/FAIL
 */
int rec3D_cleanup_ctx(recon3d_handler_t* p);

/**
 * Allocate required memory for the output volume
 * @param h
 * @return pointer to output image
 */
canvas_t * rec3D_init_canvas(recon3d_handler_t*h);

/**
 * Deallocate volume memory
 * @param h
 * @return SUCCESS/FAIL
 */
int rec3D_destroy_canvas(canvas_t**h);

/**
 * Calculate the output volume according to the reconstruction handler parameters
 * @param p reconstruction handler
 * @param canvas volume memory area
 * @param pstream pointer to input streams array
 * @param streams number of streams
 * @param normcarrier normalized carrier (fcarrier/fADC)
 * @return SUCCESS/FAIL
 */
int rec3D_reconstruct(recon3d_handler_t*p ,  canvas_t* , recon_stream_t * pstream, int streams, float normcarrier);

/**
 * @brief This function emulate the reconstruction algorithm without create an actual volume\n
 * This function must be called after canvas/stream range set and before try an actual volume reconstruction\n
 * In order to speedup the volume reconstruction, the actual algorithm doesn't check if the stream indexes (calculated for each image voxel) is inside the stream's boundaries\n
 * Hence the user must ensure that volume boundaries and stream length must be properly set\n
 * \n
 * The function calculates,for each volume voxel, the index of sample for each stream, check if the index is inside the stream's boundaries and return the out-of-bound index\n
 * \n
 * If the OOB indexes are different from zero, means that there are some image's voxel that could not be evaluated with the current acquisition stream setting\n
 * Users must trim canvas area or acquisition range to allow the correct computation of each voxel
 *
 * @param ph
 * @param pstream
 * @param numStreams
 * @param lower_oob
 * @param upper_oob
 * @return SUCCESS or DRYRUN_OOB (detected out-of-bound stream access)
 */
int rec3D_reconstruct_dryrun(recon3d_handler_t*ph , recon_stream_t * pstream, int numStreams, int *lower_oob, int *upper_oob);


/**
 * Change the reconstruction algorithm
 * @param p
 * @param newAlgo
 * @return  SUCCESS/FAIL
 */
int rec3D_setAlgo(recon3d_handler_t* p, recalgs_t newAlgo);


/**
 * @def m3DreadPixel(canvas, ph, rhoi, eli, thetai)
 * Access volume voxel given rho/elevation/azimuth indexes
 */
#define m3DreadPixel(canvas, ph, rhoi, eli, thetai) \
	(canvas[rhoi + ph->rho_samples*(eli+ ph->el_samples*thetai)])


#define REC3D_SUCCESS 0
#define REC3D_FAIL -1
#define REC3D_ERR_ALLOC -2
#define REC3D_DRYRUN_OOB -3



#endif /* RECONSTRUCTION_3D_H_ */
