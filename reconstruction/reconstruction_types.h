/**
 * @file reconstruction_types.h
 *
 *  Created on: Dec 12, 2023\n
 *  Author: ARIA Sensing\n
 */

#ifndef RECONSTRUCTION_TYPES_H_
#define RECONSTRUCTION_TYPES_H_

#include "fe_math.h"
#include "stdbool.h"

typedef float canvas_t;

typedef enum _recalgs_t {RA_DAS, RA_DMAS}recalgs_t;

#define LIGHT_SPEED 299792458.0f

typedef struct _antenna_pos_t{
	float x;
	float y;
	float delay_ns;
	float ampl_corr;
}antenna_pos_t;

typedef struct _stream_data_t{
	complex_float * pdata;
	uint32_t samples;
	float delay_corr_bins;
}stream_data_t;

typedef struct _recon_stream_t{
	complex_float * pdata;
	int size;
	float antennas_dx;	//x difference TxRx
	float antennas_dy;	//y difference TxRx
	float offset;		//stream offset in samples (combination of acquisition offset and antenna delay)
	float amplitude;	//amplitude compensation (i.e polarity correction)
}recon_stream_t;



#endif /* RECONSTRUCTION_TYPES_H_ */
