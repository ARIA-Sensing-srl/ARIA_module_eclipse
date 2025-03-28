/*
 * utils_instr_cnt.c
 *
 *  Created on: mar 05, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#include "cv32e40_core.h"
#include "utils_instr_cnt.h"

#define DEF_MHMPCNT_ID 3

void iCnt_enable(){
	hal_spr_read_then_clr(CSR_MCOUNTINHIBIT, 1);
}
void iCnt_disable(){
	hal_spr_read_then_set(CSR_MCOUNTINHIBIT, 1);
}
uint32_t iCnt_getInstrCount(){
	uint32_t cnt = hal_spr_read(CSR_MCYCLE);
	return cnt;
}

void iEvt_enable(uint8_t evt){
	hal_spr_write(CSR_MHPMEVENT(DEF_MHMPCNT_ID), (1 << evt));
	hal_spr_read_then_clr(CSR_MCOUNTINHIBIT, (1 << DEF_MHMPCNT_ID));
}
void iEvt_disable(){
	hal_spr_read_then_set(CSR_MCOUNTINHIBIT, (1 << DEF_MHMPCNT_ID));
}
uint32_t iEvt_getCount(){
	uint32_t cnt = hal_spr_read(CSR_MHPMCOUNTER(DEF_MHMPCNT_ID));
	return cnt;
}


