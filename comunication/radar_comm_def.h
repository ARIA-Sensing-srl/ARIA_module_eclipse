/*
 * radar_comm_def.h
 *
 *  Created on: Sept 02, 2020
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef RADAR_COMM_DEF_H
#define RADAR_COMM_DEF_H




//sequence command
#define mRDRCOM_decode_sequence_tx(x) ((x & 0xF0) >> 4)
#define mRDRCOM_decode_sequence_rx(x) ((x & 0x0F))
#define mRDRCOM_encode_sequence(t,r) (((t & 0xF)<<4) | (r & 0xF))
//antenna array macro command
#define mRDRCOM_antarrayID_isRx(id) (id < 4)
#define mRDRCOM_decode_antArrayID(id) ((id > 3) ? (id-4) : id)
#define mRDRCOM_encode_antArrayID(id, isRx) ((isRx) ? id : (id+4))
#define RDRCOM_STRUCT_PAYLOAD_MAX 255

//bitmaps (option procedure)
#define RDCOM_OPTPROC_BYTE1_DET_ALGO_POS 0
#define RDCOM_OPTPROC_BYTE1_CPLX_IMAGE_POS 1
#define RDCOM_OPTPROC_BYTE1_EXE_RECONSTRUCTION 2


/*
 * RADARCOMM_GET_DATA_MULTIPLE
 * Header
 * Bytes	: Description
 * 0-1		: u16, number of samples for each scan
 * 2-3		: u16, from bit 0-11 is elapesd time, bit 12-15 is the data format according to FMT encoding
 * 4-5		: u16, total number of samples expected, this number is equal to NumSamples*numScan
 * 5-end	: data, complex data are transferre as In,Qn, In+1, Qn+1 .... according to format specified into the FMT field
 * */


typedef enum {
		    RADARCOMM_UNKNOWN_COMMAND ,
			RADARCOMM_NOOP ,							// starting point
			RADARCOMM_START ,							// code = 's', no payload
			RADARCOMM_STOP ,							// code = 'o'
			RADARCOMM_GET_DATA ,						// code = 'd'
			RADARCOMM_SET_FRAME_RATE ,					// code = 'f' followed by uint16 (frame per second, max 500)
			RADARCOMM_SET_TX_POWER   ,					// code = 'p' followed by 0-7 0xFF to avoid modifications (uint8)
			RADARCOMM_SET_MIN_XRANGE ,					// code = 'x' followed by float 0xFFFFFFFF to avoid modifications(max 2048)
			RADARCOMM_SET_MAX_XRANGE ,					// code = 'X' followed by float 0xFFFFFFFF to avoid modifications(max 2048)
			RADARCOMM_SET_OFFSET 	,					// code = '0' followed by float 0xFFFFFFFF to avoid modifications(max 2048)
			RADARCOMM_SET_ITERATIONS ,					// code = 'i' followed by uint16 0xFFFF to avoid modifications
			RADARCOMM_RESET			 ,					// code = '!' no payload
			RADARCOMM_SET_DECLUT_LENGTH , 				// code = 'l', followed by uint16_t (0xFFFF to avoid modifications), 0 disable algorithm
			RADARCOMM_SET_ELAB ,						// code = 'e', followed by unit8_t (< 0x04 to avoid modifications)
			RADARCOMM_SET_OPTPROC ,						// code 'b', struct 2 bytes elaboration options (1 enable), byte0: bitmap 0:dc remove, 1:correlation, 2:matchfilter
														// byte1: bitmap 0:detection algo, 1:cplx image
			RADARCOMM_SET_DATA_CHANNEL,					// code '$' uint8_t select channel to send to host (0-n, 0xFE all,  0xFF for query)
			RADARCOMM_SET_ANGLE,						// code 'h', float steering angle (between -180 to 180)
			RADARCOMM_SET_CALIBRATION,					// code '#', calibration data (6 float) dt2 (bins), dr2 (bins), ampl[4] (relative) (0xFFFFFFFF) for read
			RADARCOMM_GET_DETECTION,					//code 'E', optional arg u8 :type(0: blob, 1 :filtered), default is filtered, return a list of target (rho, theta)
			RADARCOMM_GET_VERSION ,						//'0x01', no payload
			RADARCOMM_SET_MULTIPLE_ANGLEs,				//'0x05', STRUCT type, u8 numItems, float itemlist
			RADARCOMM_GET_DATA_MULTIPLE,				//code = '0x07' no args
			RADARCOMM_GET_OUTPUT_SIZE,					//code 0x09, no args, return u8 numStreams, u8 format, u16 samples/stream
			RADARCOMM_GET_2D,							//data are Xbins u16, Ybins u16, X*Ybins float32
			RADARCOMM_SET_SEQUENCE,						//code 0x08, set channel sequence u8: numseq, array u8: 7-4 txmask, 3-0 rxmask, numseq 0xFF : get
			RADARCOMM_SET_DATA_FMT,						//code 0x0A, u8: fmt code (see app.c)
			RADARCOMM_SET_FREQ_CARRIER,					//code  0x0B, u16, carrier freq MHz, 0xFFFF query
			RADARCOMM_SET_BW,							//code 0x0C, u16, signal BW, 0xFFFF query
			RADARCOMM_SET_CODE,							//code 0x0D, structu, array i8, code +1,-1,0 size 0xFF request
			RADARCOMM_SET_GAIN,							//code 0x0E, struct , u16 gainI, u16 gainQ, 0xFF request
			RADARCOMM_GET_IMG,							//code 0x0F, follow the same structure of data request header is "column", "etime", "row"
			RADARCOMM_SET_ANT_ARRAY,						// code 'v': set/get antenna array, set is item based, could be variable: structure is u8:antID(rx0-3, tx 4-7): f32: xpos_m, f32 ypos_m;, f32 delay)
			RADARCOMM_GET_SAMPLING_RATE_MHZ,
			RADARCOMM_SET_CANVAS,						//code:m set/get canvas u8 algo (all are f32) rhoMin, rhostep, rhoMax, phiMin(deg) ,phistep, phiMax(deg): get 0xFFFFFFFF
														//in 3D mode: rhoMin, rhostep, rhoMax, elMin(deg) ,elstep, elMax(deg), thetaMin(deg) ,thetastep, thetaMax(deg)
			RADARCOMM_SG_BW_MODE,					//arg u8: 0: default mode PLL at 1792 (max BW 1.3GHz), 1: high bw mode, pll a 2.4GHz
			RADARCOMM_SG_BAUDRATE,					//arg u32
			RADARCOMM_SG_PWRMODE,
}radar_command;




typedef struct __attribute__((packed)){
	uint8_t size;
	uint8_t buffer[RDRCOM_STRUCT_PAYLOAD_MAX]; //larger than original (16) need more data to be transferred in structured
}data_struct_t;

typedef union __attribute__((packed)) {
	uint32_t	data_uint32;
	uint16_t    data_uint16;
	uint8_t		data_uint8;
	float		data_float;
	data_struct_t data_struct;
}radar_command_payload;

typedef enum __attribute__((packed)) _payloadType_t{
	PT_NO=0,
	//regular response
	PT_U8=1,
	PT_U16=2,
	PT_U32=3,
	PT_FLOAT=4,
	PT_STRUCT=5,
	//special response (query with regular value, response with special format
	PT_U8_SR=6,
	PT_U16_SR=7,
	PT_U32_SR=8,
	PT_FLOAT_SR=9,
	PT_STRUCT_SR=10,
}payloadType_t;

typedef int (*pcomm_cb)(radar_command , radar_command_payload*, void *);

typedef struct _RadarCommandItem{
	unsigned char cmdCode;	 //code of the command
	radar_command cmd;
	//radar_response rsp;
	payloadType_t payloadType;
	pcomm_cb queryDataFunc;
}RadarCommandItem_t;

#endif
