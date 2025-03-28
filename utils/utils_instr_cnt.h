/*
 * utils_instr_cnt.h
 *
 *  Created on: mar 05, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef UTILS_UTILS_INSTR_CNT_H_
#define UTILS_UTILS_INSTR_CNT_H_

#include "stdint.h"


#define IEVT_CNT_CYCLES 		0		//Number of cycles
#define IEVT_CNT_INSTR	 		1		//instruction retired
#define IEVT_CNT_LD_STALL		2		//load use hazard
#define IEVT_CNT_JMP_STALL		3		//jump register hazard
#define IEVT_CNT_IMISS			4		//cycle waiting instruction fetch (jump/branch excluded)
#define IEVT_CNT_LD				5		//number of load instruction
#define IEVT_CNT_ST				6		//number of store instruction
#define IEVT_CNT_JUMP			7		//number of jump (unconditional)
#define IEVT_CNT_BRANCH			8		//number of branch (conditional)
#define IEVT_CNT_BRANCH_TAKEN 	9		//number of branch taken (conditional)
#define IEVT_CNT_COMP_INSTR	 	10		//number of compressed instruction retired
#define IEVT_CNT_PIPE_STALL		11		//Cycle from stalled pipeline
#define IEVT_CNT_APU_TYPE		12		//Number of type conflict on APU/FP
#define IEVT_CNT_APU_CONT		13		//Number of contention on APU/FP
#define IEVT_CNT_APU_DEP		14		//Number of dependency stall on APU/FP
#define IEVT_CNT_WB				15		//number of writeback on APUB/FP

#define miCntMeasTime(variable, function) { \
		variable = iCnt_getInstrCount(); \
		function; \
		variable = iCnt_getInstrCount()-variable; \
	}

#define miCntCycAndEvt(varCyc, varStall, function) { \
		varStall = iEvt_getCount(); \
		varCyc = iCnt_getInstrCount(); \
		function; \
		varStall = iEvt_getCount()-varStall; \
		varCyc = iCnt_getInstrCount()-varCyc; \
	}

#define miCntMeasTimeMax(variable, function) { 		\
		variable = iCnt_getInstrCount(); 			\
		function; 									\
		variable = iCnt_getInstrCount()-variable; 	\
		if (variable > variable##Max){ 				\
			variable##Max = variable; 				\
		} 											\
	}


void iCnt_enable();
void iCnt_disable();
void iEvt_enable(uint8_t evt);
void iEvt_disable();
uint32_t iCnt_getInstrCount();
uint32_t iEvt_getCount();


#endif /* UTILS_UTILS_INSTR_CNT_H_ */
