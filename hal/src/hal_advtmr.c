/*
 * hal_advtmr.c
 *
 *  Created on: Nov 6, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#include "hal_advtmr.h"
#include "stddef.h"


#define mCheckCHID(id) (id < 4)
#define mCheckEvent(evtID) (evtID < 4)
#define mCheckTmrID(id) (id < 4)


adv_tmr_t * hal_advtmr_getPeripheralHandler(uint8_t TmrID){
	adv_tmr_t * pret = NULL;
	switch (TmrID){
		case HAL_ADVTMR0_ID:
			pret = ADV_TMR0;
			break;
		case HAL_ADVTMR1_ID:
			pret = ADV_TMR1;
			break;
		case HAL_ADVTMR2_ID:
			pret = ADV_TMR2;
			break;
		case HAL_ADVTMR3_ID:
			pret = ADV_TMR3;
			break;
		default:
			break;
	}
	return pret;
}

int hal_advtmr_configure(adv_tmr_t* p, hal_advtmr_cfg_t cfg){
	hal_advtmr_stop(p);
	hal_advtmr_rst(p);
	p->CONFIG = cfg.trigmode << ADV_TMR_CONFIG_INSEL_POS | \
				cfg.clksrc << ADV_TMR_CONFIG_CLKSEL_POS  | \
				cfg.mode << ADV_TMR_CONFIG_MODE_POS | \
				cfg.prescaler << ADV_TMR_CONFIG_PRESC_POS;
	p->CNTTHR = cfg.lowcount << ADV_TMR_CNTTHR_LO_POS | \
				cfg.upcount << ADV_TMR_CNTTHR_HI_POS;
	hal_advtmr_update(p);
	return HAL_ADVTMR_SUCCESS;
}

int hal_advtmr_ch_configure(adv_tmr_t* p, int channelID, hal_advtmr_ch_cfg_t cfg){
	if (!mCheckCHID(channelID)){
		return HAL_ADVTMR_FAIL;
	}
	p->TH_CH[channelID] = cfg.mode << ADV_TMR_TH_CH_MODE_POS |
							cfg.threshold << ADV_TMR_TH_CH_THR_POS;
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_ch_setthreshold(adv_tmr_t*p, int channelID, uint16_t threshold){
	if (!mCheckCHID(channelID)){
		return HAL_ADVTMR_FAIL;
	}
	uint32_t th_ch = p->TH_CH[channelID];
	th_ch = (th_ch & ~(ADV_TMR_TH_CH_THR_MASK)) | (threshold << ADV_TMR_TH_CH_THR_POS);
	p->TH_CH[channelID] = th_ch;
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_arm(adv_tmr_t*p){
	p->CMD = 1 << ADV_TMR_CMD_ARM_POS;
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_rst(adv_tmr_t*p){
	p->CMD = 1 << ADV_TMR_CMD_RST_POS;
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_start(adv_tmr_t*p){
	p->CMD = 1 << ADV_TMR_CMD_START_POS;
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_stop(adv_tmr_t* p){
	p->CMD = 1 << ADV_TMR_CMD_STOP_POS;
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_update(adv_tmr_t* p){
	p->CMD = 1 << ADV_TMR_CMD_UPDATE_POS;
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_evt_cfg(int advtmr_evt, int advtmr_evtsource){
	if (!mCheckEvent(advtmr_evt))
		return HAL_ADVTMR_FAIL;
	uint32_t curEvt = ADV_TMR_EVNT;
	curEvt = (curEvt & ~mADV_TMR_EVNT_EVTx_MASK(advtmr_evt)) |
				mADV_TMR_EVNT_EVTx_SRC(advtmr_evt, advtmr_evtsource);
	ADV_TMR_EVNT = curEvt;
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_evt_enable(int advtmr_evt){
	if (!mCheckEvent(advtmr_evt))
		return HAL_ADVTMR_FAIL;
	ADV_TMR_EVNT |= mADV_TMR_EVNT_ENx(advtmr_evt);
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_evt_disable(int advtmr_evt){
	if (!mCheckEvent(advtmr_evt))
		return HAL_ADVTMR_FAIL;
	ADV_TMR_EVNT &= ~mADV_TMR_EVNT_ENx(advtmr_evt);
	return HAL_ADVTMR_SUCCESS;
}

int hal_advtmr_cg_enable(int advtmrID){
	if (!mCheckTmrID(advtmrID))
		return HAL_ADVTMR_FAIL;
	ADV_TMR_CG |= mADV_TMR_CG_TMRx(advtmrID);
	return HAL_ADVTMR_SUCCESS;
}
int hal_advtmr_cg_disable(int advtmrID){
	if (!mCheckTmrID(advtmrID))
		return HAL_ADVTMR_FAIL;
	ADV_TMR_CG &= ~mADV_TMR_CG_TMRx(advtmrID);
	return HAL_ADVTMR_SUCCESS;
}

