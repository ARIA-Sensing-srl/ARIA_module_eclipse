/*
 * hal_intc.c
 *
 *  Created on: Jan 23, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#include "hal_intc.h"



_hal_intc_result hal_intc_enable(int id){
	ITC->IERSET = (1 << id);
	return HAL_ITC_SUCCESS;
}
_hal_intc_result hal_intc_disable(int id){
	ITC->IERCLR = (1 << id);
	return HAL_ITC_SUCCESS;
}
bool hal_intc_status(int id){
	return (ITC->ISR & (1 << id) != 0);
}
_hal_intc_result hal_intc_clear(int id){
	ITC->ISRCLR = (1 << id);
	return HAL_ITC_SUCCESS;
}
