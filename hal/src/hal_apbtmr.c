/*
 * hal_apbtmr.c
*
 *  Created on: Jan 23, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#include "hal_apbtmr.h"

_hal_apbtr_result_t hal_apbtmr_config(volatile apb_timer_t* ptmr, hal_apb_config_t config){
	uint32_t cfg;
	cfg = 	((uint32_t)config.cascode << APB_TMR_CFG_CASC_BIT) | \
			((uint32_t)config.clksrc << APB_TMR_CFG_CCFG_BIT) | \
			((uint32_t)config.prescaler << APB_TMR_CFG_PVAL0_BIT) | \
			((uint32_t)config.precsaler_en << APB_TMR_CFG_PEN_BIT);
	switch(config.mode){
		case HAL_APBTMR_MODE_CONT:
			cfg |= (1 << APB_TMR_CFG_MODE_BIT);
			break;
		case HAL_APBTMR_MODE_ONESHOT:
			cfg |= (1 << APB_TMR_CFG_ONES_BIT) | (1 << APB_TMR_CFG_MODE_BIT);
			break;
		default:
			return HAL_APBTMR_FAIL;
			break;
	}
	if (config.tmrhilo == HAL_APBTMR_TMR_LO){
		ptmr->CFG_LO = 0;
		while(ptmr->CFG_LO);
		ptmr->RESET_LO = 1;
		asm volatile("nop");
		asm volatile("nop");
		ptmr->CFG_LO = cfg;
		ptmr->CMP_LO = config.compare;
		ptmr->CNT_LO = 0;
	}else{
		ptmr->CFG_HI = 0;
		while(ptmr->CFG_HI);
		ptmr->RESET_HI = 1;
		asm volatile("nop");
		asm volatile("nop");
		ptmr->CFG_HI = cfg;
		ptmr->CMP_HI = config.compare;
		ptmr->CNT_HI = 0;
	}

	return HAL_APBTMR_SUCCESS;
}
_hal_apbtr_result_t hal_apbtmr_reset(volatile apb_timer_t* ptmr, uint8_t hilo){
	if (hilo == HAL_APBTMR_TMR_LO)
		ptmr->RESET_LO = 1;
	else
		ptmr->RESET_HI = 1;
	return HAL_APBTMR_SUCCESS;
}
_hal_apbtr_result_t hal_apbtmr_start(volatile apb_timer_t* ptmr, uint8_t hilo){
	if (hilo == HAL_APBTMR_TMR_LO)
		ptmr->START_LO = 1;
	else
		ptmr->START_HI = 1;
	return HAL_APBTMR_SUCCESS;
}

_hal_apbtr_result_t hal_apbtmr_stop(volatile apb_timer_t* ptmr, uint8_t hilo){
	if (hilo == HAL_APBTMR_TMR_LO)
		ptmr->CMP_LO &= ~(1 << APB_TMR_CFG_EN_BIT);
	else
		ptmr->CMP_HI &= ~(1 << APB_TMR_CFG_EN_BIT);
	return HAL_APBTMR_SUCCESS;
}


_hal_apbtr_result_t hal_apbtmr_int(volatile apb_timer_t* ptmr, uint8_t hilo, uint8_t enable){
	volatile uint32_t *pcfg;
	pcfg = (hilo == HAL_APBTMR_TMR_LO) ? &(ptmr->CFG_LO) : &(ptmr->CFG_HI);
	if (enable)
		*pcfg |= (1 << APB_TMR_CFG_IRQEN_BIT);
	else
		*pcfg &= ~(1 << APB_TMR_CFG_IRQEN_BIT);
	return HAL_APBTMR_SUCCESS;
}

_hal_apbtr_result_t hal_apbtmr_int_pending_clear(volatile apb_timer_t* ptmr, uint8_t hilo){
	if (hilo == HAL_APBTMR_TMR_LO)
		APB_TMR->IRQRST_LO = (1 << APB_TMR_IRQRST_BIT);
	else
		APB_TMR->IRQRST_HI = (1 << APB_TMR_IRQRST_BIT);
	return HAL_APBTMR_SUCCESS;
}



