/*
 * ipc_commands.h
 *
 *  Created on: Dec 12, 2023
 *      Author: ARIA Sensing
 */

#ifndef IPC_COMMANDS_H_
#define IPC_COMMANDS_H_

#include "stdint.h"
#include "ipc_types.h"

/* IPC command specificatio
 * The coprocessor plays as a slave, the CV is a master
 * For small paramater (size < 4) the content is handled by the ipc_messagedata_t
 * For large parameters (> 4) the sender allocate the memory and provide the pointer, the receiver use the pointer and fill the memory
 * The allocated memory is valid only during the transaction (exception for data buffers for frames)
 *
 */


//default response
#define IPC_RSP_SUCCESS 0

//response if the error state is asserted
#define IPC_RSP_ERR_UNSUPPORTED 1	//unknown command
#define IPC_RSP_ERR_INVALID 2		//command could not be executed
#define IPC_RSP_ERR_HANDLING 3		//command hanlder returned an error

//IPC_CMD_GET_STATE
#define IPC_RSP_STATE_INITIALIZATION 	1	//system initialization is ongoing
#define IPC_RSP_STATE_READY 			2	//system ready
#define IPC_RSP_STATE_ERROR				3	//error state
#define IPC_RSP_STATE_PD				4	//power down

//power down modes

#define IPC_CMD_ARG_PD_DISABLE			0	//no powerdown, system is always on
#define IPC_CMD_ARG_PD_0				1	//pwrsave mode 1
#define IPC_CMD_ARG_PD_1				2	//pwrsave mode 2
#define IPC_CMD_ARG_PD_2				3	//pwrsave mode 3

//acquisiton mode
#define IPC_CMD_ARG_ACQUISITION_MODE_SINGLE	0
#define IPC_CMD_ARG_ACQUISITION_MODE_CONT	1

//postprcessing options (mask)
#define IPC_CMD_ARG_POSTPROC_DCREM_MASK 	1	//data returned are signed
#define IPC_CMD_ARG_POSTPROC_CORR_MASK	 	2	//data returned are singed
#define IPC_CMD_ARG_POSTPROC_MATCHFILT_MASK	4	//data returned are singed

#define IPC_CMD_ARG_POSTPROC_ALL			(	IPC_CMD_ARG_POSTPROC_DCREM_MASK | \
												IPC_CMD_ARG_POSTPROC_CORR_MASK | \
												IPC_CMD_ARG_POSTPROC_MATCHFILT_MASK)




//list the IPC commands code
//first code set for system management and auxiliar function
#define IPC_CMD_GET_STATE 		0		//ARG:none, RESP:Y, return the system state
#define IPC_CMD_SET_PD			1		//ARG:Y,	RESP:Y, set the powerdown mode
#define IPC_CMD_SET_XTCLK		2		//ARG:u32 freq Hz,	RESP:Y, return success or failure
#define IPC_CMD_DATA_MOVER		3		//arg:ipc_cmd_datamover_t*, RESP: NO, helper command to aid processor for transfer data
#define IPC_CMD_SET_FBLINES		4		//ARG:u32 encoded 0-8 txFb, 8-16 rxFb, response yes
#define IPC_CMD_PLL_UPDATE		5		//no arg

//from 256 the command for radar configuration (even code for set ops, odd code for get ops)
#define IPC_CMD_SET_TXPLL_FREQ			256		//ARG:u32, freqMHz, RESP:Y, return u32 actualfreq in MHz
#define IPC_CMD_GET_TXPLL_FREQ			257		//ARG:no, RESP:Y, return u32 actualfreq in MHz
#define IPC_CMD_SET_TXDLL_ENV			258		//ARG:u32, bbenv code, RESP:Y, return u8 actualfreq bbenv setting
#define IPC_CMD_GET_TXDLL_ENV			259		//ARG:none, RESP:Y, return u32 actualfreq bbenv setting
#define IPC_CMD_SET_ITERATIONS			260		//arg:u16 iterations, RESP:Y, return u16 current value
#define IPC_CMD_GET_ITERATIONS			261		//arg:none , RESP:Y, return u16 current value
#define IPC_CMD_SET_ACQUISITION_MODE	262		//arg:u8, acqmode, RESP:Y, return u8 current acq mdoe
#define IPC_CMD_GET_ACQUISITION_MODE	263		//arg:none, RESP:Y, return u8 current acq mdoe
#define IPC_CMD_SET_FPS					264		//arg:u16, FPS, RESP:Y, return u16 current FPS
#define IPC_CMD_GET_FPS					265		//arg:none, FPS, RESP:Y, return u16 current FPS
#define IPC_CMD_SET_TXPWR				266		//arg:u8 pwr, RESP:Y, return u8 current TxPwr
#define IPC_CMD_GET_TXPWR				267		//arg:none, RESP:Y, return u8 current TxPwr
#define IPC_CMD_SET_LNA_FREQ			268		//arg:u8 lna_freq, RESP:Y, return u8 current lna_freq
#define IPC_CMD_GET_LNA_FREQ			269		//arg:none, RESP:Y, return u8 current lna_freq
#define IPC_CMD_SET_VGA_GAIN			270		//arg:u16/u16 vga_gain (upper I lower Q), RESP:Y, return u32 current vga_gain
#define IPC_CMD_GET_VGA_GAIN			271		//arg:none, RESP:Y, return u32 current vga_gain
#define IPC_CMD_SET_CODEWORD			272		//arg:ipc_cmd_codeword_t*, RESP:Y, return success or vailure
#define IPC_CMD_GET_CODEWORD			273		//arg:ipc_cmd_codeword_t*, RESP:Y, return success or failure
#define IPC_CMD_SET_CORR_REF			274		//arg:ipc_cmd_corr_reference_t*, RESP:Y, return success or failure
#define IPC_CMD_GET_CORR_REF			275		//arg:ipc_cmd_corr_reference_t*, RESP:Y, return success or failure
#define IPC_CMD_SET_POSTPROC_OPT		276		//arg:option mask, RESP:Y, return current processing option
#define IPC_CMD_GET_POSTPROC_OPT		277		//arg:none, RESP:Y, return current processing option
#define IPC_CMD_SET_DATASCALE			278		//arg:scale factor (shift amount, positive for right, RESP:Y, return current scale
#define IPC_CMD_GET_DATASCALE			279		//arg:none, RESP:Y, return current scale
#define IPC_CMD_SET_XRANGE				280		//arg:ptarray_i32 xmin, xmax, RESP:Y, return value
#define IPC_CMD_GET_XRANGE				281		//arg:ptarray_i32, RESP:Y, return value



//from 4096 start the radar commands
#define IPC_CMD_START_SINGLE			4096	//arg:ipc_cmd_acq_t*, RESP:Y, return success/or error state
#define IPC_CMD_START_CONT				4097	//arg:none, RESP:Y, return success or error
#define IPC_CMD_STOP_CONT				4098	//arg:none, RESP:Y, return success or error
#define IPC_CMD_SET_PP_BUFFER0			4099	//arg:ipc_cmd_acq_t* set pingpong buffer0, RESP:Y, return success or error
#define IPC_CMD_SET_PP_BUFFER1			4100	//arg:ipc_cmd_acq_t* set pingpong buffer1, RESP:Y, return success or error
#define IPC_CMD_SET_ACQ_COMPLETE_EVT	4101	//arg:event number (1-8, 0 disable), RESP:Y, return success, when an acquisition is completed, the processor trig an APB SW event
#define IPC_CMD_SET_PP_BUFFER0_RDY_EVT	4102	//arg:event number (1-8, 0 disable), RESP:Y, return success, when buffer0 is filled, the processor trig an APB SW event
#define IPC_CMD_SET_PP_BUFFER1_RDY_EVT	4103	//arg:event number (1-8, 0 disable), RESP:Y, return success, when buffer1 is completed, the processor trig an APB SW event
#define IPC_CMD_SET_OVERRUN_EVT			4104	//arg:event number (1-8, 0 disable), RESP:Y, return success, when an overrun occur (next acquisition ready but no buffer available), the processor trin an APB SW event
#define IPC_CMD_GET_FRAMEBINS			4105	//arg: none, return number of bins per frame

//in this region even codes are for SET, odd codes are for GET
#define IPC_REGION_SYSTEM_MNGR_START	0
#define IPC_REGION_SYSTEM_MNGR_END		255

#define IPC_REGION_SETGET_START 		256
#define IPC_REGION_SETGET_END			511

#define IPC_REGION_RESEVED_START		512
#define IPC_REGION_RESEVED_END			1023

#define IPC_REGION_RDR_CMD_START		4096
#define IPC_REGION_RDR_CMD_END			5119

#define IPC_EVT_CODE_RANGE_MIN 			0
#define IPC_EVT_CODE_RANGE_MAX 			8

#define mIPCSGisGet(x) ((x & 0x1UL) == 1)
#define mIPCSGisSet(x) ((x & 0x1UL) == 0)

//convert base command into get or set
#define mIPCSG2Set(x)	((x) & ~1)
#define mIPCSG2Get(x)	((x) | 1)

#define mIPCVGAGainEncode(gi, gq) ((gi) << 16 | (gq) )
#define mIPCVGAGainDecodeI(x) (((x) >> 16) & 0xFFFF)
#define mIPCVGAGainDecodeQ(x) ((x) & 0xFFFF)

//EvtCode is different
#define mIPCEvtEncode(x) ((x))
#define mIPCEvtDecode(x) ((x))

#endif /* IPC_COMMANDS_H_ */
