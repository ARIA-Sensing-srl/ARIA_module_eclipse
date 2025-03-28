/*
 * hydrogen_UD_v1.c
 *
 *  Created on: Jan 25, 2024
 *      Author: ARIA Sensing

 */

#include "ipc.h"
#include "ipc_commands.h"
#include "hydrogen_UD_v1.h"
#include "stdlib.h"
#include "string.h"


//** DEFINES START **//
#define DEF_INIT_VGA_GAIN_IQ 1
#define DEF_INIT_BW_MHZ 900
#define DEF_INIT_FC_MHZ 8064
#define DEF_INIT_ITERATIONS 100
#define DEF_INIT_FPS 10
#ifdef DEBUG
#define DEF_IPCIO_TIMEOUT_ms 0
#else
#define DEF_IPCIO_TIMEOUT_ms 1000
#endif
#define DEF_IPCIO_LLCONFIG_TIMEOUT_ms 1000
#define DEF_INIT_XOFFSET 0.0f
#define DEF_INIT_XMIN 0.0f
#define DEF_INIT_XMAX 5.0f
#define DEF_INIT_ACQMODE HUACQ_SINGLE
#define DEF_INIT_PWR HUTXPWR_HIGH
#define DEF_INIT_CODE {1,0,0}
//** DEFINES END **//

//** MACRO START **//
//** MACRO END **//

//** TYPES START **//
//** TYPES END **//

//** PRIVATE VARIABLES START **//
//** PRIVATE VARIABLES END **//

//** PUBLIC VARIABLES START **//
//** PUBLIC VARIABLES END **//

//** PRIVATE FUNCTIONS START **//
static int priv_ipcio_blocking(HydrUDriver_t *, ipc_messagedata_t*, uint32_t command, uint32_t timeout); //perform IPCIO, wait
static int priv_update_framebins(HydrUDriver_t *);
//** PRIVATE FUNCTIONS END **//

//** PUBLIC FUNCTIONS START **//
//** PUBLIC FUNCTIONS END **//


int HydrUDrv_init(HydrUDriver_t * pdrv){
	memset((void*)pdrv,0,sizeof(HydrUDriver_t));
	pdrv->ID = 0; //for now it's only a placeholder
	//range and bins updated on configure;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_configure(HydrUDriver_t * pdrv, uint32_t xtalfreq_Hz, uint8_t txfbMask, uint8_t rxfbMask){
	//Peripheral need the frequency of the XTAL to calculate all
	ipc_messagedata_t ipcmsg;

	//don't switch the sequence
	//set feedback lines
	ipcmsg.u32 = (uint32_t)txfbMask | (((uint32_t)rxfbMask) << 8);
	if (priv_ipcio_blocking(pdrv, &ipcmsg, IPC_CMD_SET_FBLINES, DEF_IPCIO_TIMEOUT_ms))
		return HYDRUDRV_FAIL;

	ipcmsg.u32 = xtalfreq_Hz;
	if (priv_ipcio_blocking(pdrv, &ipcmsg, IPC_CMD_SET_XTCLK, DEF_IPCIO_TIMEOUT_ms))
		return HYDRUDRV_FAIL;
	//wait for initialization complete
	uint32_t startticks = pdrv->pGetSysTick();
	for(;;){
		if (priv_ipcio_blocking(pdrv, &ipcmsg, IPC_CMD_GET_STATE, DEF_IPCIO_TIMEOUT_ms))
			return HYDRUDRV_FAIL;
		if (ipcmsg.u32 == IPC_RSP_STATE_INITIALIZATION){
			if ((pdrv->pGetSysTick()-startticks) > DEF_IPCIO_LLCONFIG_TIMEOUT_ms)
				return HYDRUDRV_TIMEOUT;
			continue;
		}

		//at this point response is different, check if we can proceed or not
		if (ipcmsg.u32 == IPC_RSP_STATE_READY)
			break;
		else{
			//unexpected state, exit
			return HYDRUDRV_FAIL;
		}
	}
	//init complete, start configuration
	pdrv->xoffset = DEF_INIT_XOFFSET;	//xoffset call set range, preset the value
	int ret = HydrUDrv_set_range(pdrv, DEF_INIT_XMIN, DEF_INIT_XMAX);
	ret += HydrUDrv_set_carrier(pdrv, DEF_INIT_FC_MHZ);
	ret += HydrUDrv_set_bandwidth(pdrv, DEF_INIT_BW_MHZ);
	ret += HydrUDrv_set_gain(pdrv, DEF_INIT_VGA_GAIN_IQ,DEF_INIT_VGA_GAIN_IQ);
	ret += HydrUDrv_set_framerate(pdrv, DEF_INIT_FPS);
	ret += HydrUDrv_set_acqmode(pdrv, DEF_INIT_ACQMODE);
	ret += HydrUDrv_set_pwr(pdrv, DEF_INIT_PWR);
	ret += HydrUDrv_set_integrations(pdrv, DEF_INIT_ITERATIONS);

	int8_t code[] = DEF_INIT_CODE;
	ret += HydrUDrv_set_code(pdrv, code, sizeof(code));

	if (ret)
		return HYDRUDRV_FAIL;
	return HYDRUDRV_SUCCESS;
}


int HydrUDrv_set_range(HydrUDriver_t * pdrv, float min, float max){
	if (min > max){
		return HYDRUDRV_INVALID_PARAMS;
	}
	//convert range from local scale to driver
	int32_t xrange0mm1[2];
	xrange0mm1[0] = (int32_t)((min + pdrv->xoffset)*10000.0);
	xrange0mm1[1] = (int32_t)((max + pdrv->xoffset)*10000.0);
	ipc_messagedata_t msg;
	msg.pdata = (void*)xrange0mm1;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_XRANGE, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	//complete (set back the actual parameters
	pdrv->xmin_range = ((float)(xrange0mm1[0])/10000.0)-pdrv->xoffset;
	pdrv->xmax_range = ((float)(xrange0mm1[1])/10000.0)-pdrv->xoffset;

	//update frae bins
	if (priv_update_framebins(pdrv))
		return HYDRUDRV_FAIL;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_set_carrier(HydrUDriver_t * pdrv, uint32_t freqMHz){
	if (freqMHz == 0)
		return HYDRUDRV_INVALID_PARAMS;

	ipc_messagedata_t msg;
	msg.u32 = freqMHz;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_TXPLL_FREQ, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	pdrv->freqCarrier_MHz = msg.u32;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_set_bandwidth(HydrUDriver_t * pdrv, uint32_t freqMHz){
	if (freqMHz == 0)
		return HYDRUDRV_INVALID_PARAMS;

	ipc_messagedata_t msg;
	msg.u32 = freqMHz;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_TXDLL_ENV, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	pdrv->bandwidth_MHz = msg.u32;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_set_gain(HydrUDriver_t * pdrv, uint16_t iCH, uint16_t qCH){
	ipc_messagedata_t msg;
	msg.u32 = mIPCVGAGainEncode(iCH, qCH);
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_VGA_GAIN, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	pdrv->VGA_gain[HYDRUD_DRV_CHI] = mIPCVGAGainDecodeI(msg.u32);
	pdrv->VGA_gain[HYDRUD_DRV_CHQ] = mIPCVGAGainDecodeQ(msg.u32);
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_set_framerate(HydrUDriver_t * pdrv, uint16_t FPS){
	if (!FPS)
		return HYDRUDRV_INVALID_PARAMS;
	ipc_messagedata_t msg;
	msg.u32 = (uint32_t )FPS;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_FPS, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	pdrv->frame_rate = (uint16_t)msg.u32;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_set_code(HydrUDriver_t *pdrv, int8_t * pcode, uint8_t size){
	if (!size){
		return HYDRUDRV_FAIL;
	}
	ipc_messagedata_t msg;
	ipc_cmd_codeword_t codeW;
	msg.pdata = (void*)&codeW;
	codeW.pCodeWord = pcode;
	codeW.size = size;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_CODEWORD, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	//copy to local
	pdrv->codeSize = size;
	memcpy((void*)pdrv->code, (const void*)pcode, size);
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_set_pwr(HydrUDriver_t * pdrv, HydrUDrv_pwrlvl_t pwrlvl){
	ipc_messagedata_t msg;
	msg.u32 = (uint32_t)pwrlvl;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_TXPWR, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	pdrv->txpwr = msg.u32;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_set_acqmode(HydrUDriver_t * pdrv, HydrUDrv_acqMode_t mode){
	ipc_messagedata_t msg;
	msg.u32 = (mode == HUACQ_CONT) ? 	IPC_CMD_ARG_ACQUISITION_MODE_CONT : \
										IPC_CMD_ARG_ACQUISITION_MODE_SINGLE;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_ACQUISITION_MODE, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	pdrv->acqMode = msg.u32;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_set_integrations(HydrUDriver_t * pdrv, uint16_t val){
	ipc_messagedata_t msg;
	msg.u32 = (uint32_t)val;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_ITERATIONS, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	pdrv->iterations = msg.u32;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_set_pwrSave(HydrUDriver_t * pdrv, HydrUDrv_pwrsave_t mode){

	ipc_messagedata_t msg;
	msg.u32 = (uint32_t)mode;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_PD, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_set_offset(HydrUDriver_t * pdrv, float offset){
	pdrv->xoffset = offset;
	//set ranges
	return HydrUDrv_set_range(pdrv, pdrv->xmin_range, pdrv->xmax_range);
}


int HydrUDrv_get_range(HydrUDriver_t *pdrv, float *pmin, float *pmax){
	*pmin = pdrv->xmin_range;
	*pmax = pdrv->xmax_range;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_get_carrier(HydrUDriver_t *pdrv, uint32_t *pfreqMHz){
	*pfreqMHz = pdrv->freqCarrier_MHz;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_get_bandwidth(HydrUDriver_t *pdrv, uint32_t *pfreqMHz){
	*pfreqMHz = pdrv->bandwidth_MHz;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_get_gain(HydrUDriver_t *pdrv, uint16_t *piCH, uint16_t *pqCH){
	*piCH = pdrv->VGA_gain[HYDRUD_DRV_CHI];
	*pqCH = pdrv->VGA_gain[HYDRUD_DRV_CHQ];
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_get_framerate(HydrUDriver_t *pdrv, uint16_t * pFPS){
	*pFPS = pdrv->frame_rate;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_get_framebins(HydrUDriver_t *pdrv, uint32_t * pbins){
	*pbins = pdrv->frame_bins;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_get_code(HydrUDriver_t * pdrv, int8_t *pcode, uint8_t * psize){
	if (*psize < pdrv->codeSize)
		return HYDRUDRV_INVALID_PARAMS;
	*psize = pdrv->codeSize;
	if (*psize)
		memcpy((void*)pcode, (const void*)pdrv->code, *psize);
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_get_pwr(HydrUDriver_t * pdrv, HydrUDrv_pwrlvl_t * p){
	*p = pdrv->txpwr;
	return HYDRUDRV_SUCCESS;
}
int HydrUDrv_get_acqmode(HydrUDriver_t *pdrv, HydrUDrv_acqMode_t *p){
	*p = pdrv->acqMode;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_get_integrations(HydrUDriver_t * pdrv, uint16_t * pval){
	*pval = pdrv->iterations;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_get_offset(HydrUDriver_t * pdrv, float * pval){
	*pval = pdrv->xoffset;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_start_single(HydrUDriver_t * pdrv, HydrUDrv_acqHandler_t* ph){
	if (pdrv->acqMode != HUACQ_SINGLE)
		return HYDRUDRV_INVALID_STATUS; //if interface isn't set to single, exit with error

	ipc_messagedata_t msg;
	msg.pdata = (void*)ph; //handler is the same used for the IPC, just a renaming
	*(ph->pstatus) = 0; //clear status
	int ret  = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_START_SINGLE, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_start_cont(HydrUDriver_t * pdrv, HydrUDrv_acqHandler_t* pbuf0, HydrUDrv_acqHandler_t* pbuf1){
	if (pdrv->acqMode != HUACQ_CONT)
		return HYDRUDRV_INVALID_STATUS;
	if (pbuf0 == NULL)
		return HYDRUDRV_INVALID_PARAMS;
	//ready, set buffers
	*(pbuf0->pstatus) = 0;
	ipc_messagedata_t msg;
	msg.pdata = (void*)pbuf0;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_PP_BUFFER0, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;

	HydrUDrv_acqHandler_t dummyHandler;

	if (pbuf1 == NULL){
		memset((void*)&dummyHandler,0, sizeof(dummyHandler));
		pbuf1 = &dummyHandler;
	}else{
		*(pbuf1->pstatus) = 0;
	}
	msg.pdata = (void*)pbuf1;
	ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_SET_PP_BUFFER1, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	//start
	ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_START_CONT, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	return HYDRUDRV_SUCCESS;
}

int HydrUDrv_stop_cont(HydrUDriver_t * pdrv){
	ipc_messagedata_t msg;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_STOP_CONT, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	return HYDRUDRV_SUCCESS;
}


/*
 * Send message and return the response in pmsg
 * Return negative values if internal error occur (no space, timeout)
 * 0 if success
 * > 0 IPC transaction completed in error state
 * */

static int priv_ipcio_blocking(HydrUDriver_t * pdrv, ipc_messagedata_t* pmsg, uint32_t command, uint32_t timeout){
	int iHandler = ipc_send_message(*pmsg, command, true);
	if (iHandler == IPC_NO_SPACE){
		return HYDRUDRV_FAIL;
	}
	int ret;
	ipc_item_t responseItem;

	uint32_t startTime = pdrv->pGetSysTick();
	while((ret = ipc_check_response(&responseItem, iHandler)) == IPC_NO_RESPONSE){
		if (timeout){
			if ((pdrv->pGetSysTick() - startTime) > timeout)
				break;
		}
	}
	//check reason of exit
	if (ret != IPC_SUCCESS)
		return HYDRUDRV_FAIL; //timeout or error

	//ready

	*pmsg = responseItem.msgdata;
	//check if response is error
	uint32_t ctl = mIPCGetCtl(responseItem.cmdctl);
	if (ctl == IPC_CTL_READY_ERR){
		//the payload is the message error code
		return abs(responseItem.msgdata.i32);
	}
	//response is OK return 0
	return 0;
}


static int priv_update_framebins(HydrUDriver_t * pdrv){
	ipc_messagedata_t msg;
	int ret = priv_ipcio_blocking(pdrv, &msg, IPC_CMD_GET_FRAMEBINS, DEF_IPCIO_TIMEOUT_ms);
	if (ret)
		return HYDRUDRV_FAIL;
	pdrv->frame_bins = (int16_t)msg.u32;
	return HYDRUDRV_SUCCESS;
}
