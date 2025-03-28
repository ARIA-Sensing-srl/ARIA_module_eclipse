/*
 * hal_copctl.h
*
 *  Created on: Jan 23, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#include "hal_copctl.h"
#include "socctrl.h"





	_hal_copctl_result hal_copctl_set_bootaddress(uint32_t address){
		SOCTRL_COPR->BOOTADDR = address;
		return HAL_COPCTL_SUCCESS;
	}
	_hal_copctl_result hal_copctl_rst_assert(){
		SOCTRL_COPR->CR &= ~(1<<SOCCTRL_COPR_CR_NRST_BIT);
		return HAL_COPCTL_SUCCESS;
	}
	_hal_copctl_result hal_copctl_rst_deassert(){
		SOCTRL_COPR->CR |= (1<<SOCCTRL_COPR_CR_NRST_BIT);
		return HAL_COPCTL_SUCCESS;
	}

