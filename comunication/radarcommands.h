/*
 * radarcommands.h
*
 *  Created on: Sept 02, 2020
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef COMM_RADARCOMMANDS_H_
#define COMM_RADARCOMMANDS_H_

#include "radar_comm_def.h"


//handlers, pass the original command, the payload and additional handler that could be used by the dispatcher.
extern int comm_cb_frame_rate(radar_command , radar_command_payload*, void *);
extern int comm_cb_min_Xrange(radar_command , radar_command_payload*, void *);
extern int comm_cb_max_Xrange(radar_command , radar_command_payload*, void *);
extern int comm_cb_offset(radar_command , radar_command_payload*, void *);
extern int comm_cb_iterations(radar_command , radar_command_payload*, void *);

extern int comm_cb_declu_lgth(radar_command , radar_command_payload*, void *);
extern int comm_cb_elab(radar_command , radar_command_payload*, void *);
extern int comm_cb_data_channel(radar_command , radar_command_payload*, void *);
//extern int comm_cb_angle(radar_command , radar_command_payload*, void *);

//extern int comm_cb_output(radar_command , radar_command_payload*, void *);
extern int comm_cb_tx_power(radar_command , radar_command_payload*, void *);
extern int comm_cb_output_size(radar_command , radar_command_payload*, void *);

extern int comm_cb_running(radar_command , radar_command_payload*, void *);
extern int comm_cb_stop(radar_command , radar_command_payload*, void *);
extern int comm_cb_reset(radar_command , radar_command_payload*, void *);
extern int comm_cb_unknown(radar_command , radar_command_payload*, void *);
extern int comm_cb_data(radar_command , radar_command_payload*, void *);
extern int comm_cb_version(radar_command , radar_command_payload*, void *);
extern int comm_cb_data_mult(radar_command , radar_command_payload*, void *);
extern int comm_cb_sequence(radar_command , radar_command_payload*, void *);
extern int comm_cb_format(radar_command , radar_command_payload*, void *);
extern int comm_cb_freq_carrier(radar_command , radar_command_payload*, void *);
extern int comm_cb_freq_bw(radar_command , radar_command_payload*, void *);
extern int comm_cb_code(radar_command , radar_command_payload*, void *);
extern int comm_cb_gain(radar_command , radar_command_payload*, void *);
extern int comm_cb_img(radar_command , radar_command_payload*, void *);
extern int comm_cb_antarray(radar_command , radar_command_payload*, void *);
extern int comm_cb_canvas(radar_command , radar_command_payload*, void *);
extern int comm_cb_optproc(radar_command , radar_command_payload*, void *);
extern int comm_cb_samprate(radar_command , radar_command_payload*, void *);
extern int comm_cb_bwmode(radar_command , radar_command_payload*, void *);
extern int comm_cb_baudrate(radar_command , radar_command_payload*, void *);
extern int comm_cb_pwrmode(radar_command , radar_command_payload*, void *);




static const struct _RadarCommandItem RadarCommandItem[]={
		{'s',RADARCOMM_START						,PT_NO		,comm_cb_running				},
		{'o',RADARCOMM_STOP							,PT_NO		,comm_cb_stop					},
		{'d',RADARCOMM_GET_DATA						,PT_NO		,comm_cb_data					},
		{'f',RADARCOMM_SET_FRAME_RATE				,PT_U16		,comm_cb_frame_rate				},
		{'p',RADARCOMM_SET_TX_POWER					,PT_U8		,comm_cb_tx_power				},
		{'x',RADARCOMM_SET_MIN_XRANGE				,PT_FLOAT	,comm_cb_min_Xrange				},
		{'X',RADARCOMM_SET_MAX_XRANGE				,PT_FLOAT	,comm_cb_max_Xrange				},
		{'0',RADARCOMM_SET_OFFSET					,PT_FLOAT	,comm_cb_offset					},
		{'i',RADARCOMM_SET_ITERATIONS				,PT_U16		,comm_cb_iterations				},
		{'!',RADARCOMM_RESET						,PT_NO		,comm_cb_reset					},
		{'l',RADARCOMM_SET_DECLUT_LENGTH			,PT_U16		,comm_cb_declu_lgth				},
		{'e',RADARCOMM_SET_ELAB						,PT_U8		,comm_cb_elab					},
		{'b',RADARCOMM_SET_OPTPROC					,PT_STRUCT	,comm_cb_optproc				},
		{0x01,RADARCOMM_GET_VERSION					,PT_NO		,comm_cb_version				},
		{'?',RADARCOMM_UNKNOWN_COMMAND				,PT_NO		,comm_cb_unknown				},
		{'$',RADARCOMM_SET_DATA_CHANNEL				,PT_U8		,comm_cb_data_channel			},
		{0x07,RADARCOMM_GET_DATA_MULTIPLE			,PT_NO		,comm_cb_data_mult				},
		{0x09,RADARCOMM_GET_OUTPUT_SIZE				,PT_NO		,comm_cb_output_size			},
		{0x08,RADARCOMM_SET_SEQUENCE				,PT_STRUCT	,comm_cb_sequence				},
		{0x0A,RADARCOMM_SET_DATA_FMT				,PT_U8		,comm_cb_format					},
		{0x0B,RADARCOMM_SET_FREQ_CARRIER			,PT_U16		,comm_cb_freq_carrier			},
		{0x0C,RADARCOMM_SET_BW						,PT_U16		,comm_cb_freq_bw				},
		{0x0D,RADARCOMM_SET_CODE					,PT_STRUCT	,comm_cb_code					},
		{0x0E,RADARCOMM_SET_GAIN					,PT_STRUCT	,comm_cb_gain					},
		{0x0F,RADARCOMM_GET_IMG						,PT_NO		,comm_cb_img					},
		{0x05,RADARCOMM_GET_SAMPLING_RATE_MHZ		,PT_U16		,comm_cb_samprate				},
		{'v',RADARCOMM_SET_ANT_ARRAY				,PT_STRUCT	,comm_cb_antarray				},
		{'m',RADARCOMM_SET_CANVAS					,PT_STRUCT	,comm_cb_canvas					},
		{0x03,RADARCOMM_SG_BW_MODE					,PT_U8		,comm_cb_bwmode					},
		{0x04,RADARCOMM_SG_BAUDRATE					,PT_U32		,comm_cb_baudrate				},
		{0x06,RADARCOMM_SG_PWRMODE					,PT_U8		,comm_cb_pwrmode				},
};

#define RDR_CMD_LIST_SIZE (sizeof(RadarCommandItem)/(sizeof(RadarCommandItem[0])))



#endif /* COMM_RADARCOMMANDS_H_ */
