/*
 * hal_pwm.c
 *
 *  Created on: Nov 6, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */


#include "hal_pwm.h"
#include "stddef.h"

//pwm handler encode tmrID and chID
#define mPWMH_encode(tmrID, chID) (((tmrID) << 8) | (chID))
#define mPWMH_gettmrID(handler) ((handler >> 8) & 0xFF)
#define mPWMH_getchID(handler) ((handler) & 0xFF)



hal_pwm_handler hal_pwm_config(hal_pwm_cfg_t cfg){
	hal_advtmr_cfg_t timercfg;
	timercfg.clksrc = cfg.clksrc;
	timercfg.lowcount = 0;
	timercfg.mode = ADVTMR_CNTMODE_UP;
	timercfg.prescaler = cfg.prescaler;
	timercfg.trigmode = ADVTMR_TRIGMODE_CONT;
	timercfg.upcount = cfg.period;

	//enable timer
	if (hal_advtmr_cg_enable(cfg.advTmrID))
		return HAL_PWM_HANDLER_INVALID;

	adv_tmr_t * padvtmr = hal_advtmr_getPeripheralHandler(cfg.advTmrID);
	if (padvtmr == NULL)
		return HAL_PWM_HANDLER_INVALID;
	//setup timer
	if (hal_advtmr_configure(padvtmr, timercfg))
		return HAL_PWM_HANDLER_INVALID;
	//setup channel
	hal_advtmr_ch_cfg_t chcfg;
	if (cfg.pwmmode == PWM_MODE_POL0)
		chcfg.mode = ADVTMR_CHMODE_PWM_POL0;
	else
		chcfg.mode = ADVTMR_CHMODE_PWM_POL1;
	chcfg.threshold = 0; //initial threshold set to 0
	if (hal_advtmr_ch_configure(padvtmr, cfg.advTmrChannel, chcfg)){
		return HAL_PWM_HANDLER_INVALID;
	}
	int ret = mPWMH_encode(cfg.advTmrID, cfg.advTmrChannel);
	return ret;
}
int hal_pwm_start(hal_pwm_handler h){
	adv_tmr_t * ptmr = hal_advtmr_getPeripheralHandler(mPWMH_gettmrID(h));
	return hal_advtmr_start(ptmr);
}
int hal_pwm_stop(hal_pwm_handler h){
	adv_tmr_t * ptmr = hal_advtmr_getPeripheralHandler(mPWMH_gettmrID(h));
	return hal_advtmr_stop(ptmr);
}
int hal_pwm_set(hal_pwm_handler h, uint16_t value){
	adv_tmr_t * ptmr = hal_advtmr_getPeripheralHandler(mPWMH_gettmrID(h));
	return hal_advtmr_ch_setthreshold(ptmr, mPWMH_getchID(h), value);
}
