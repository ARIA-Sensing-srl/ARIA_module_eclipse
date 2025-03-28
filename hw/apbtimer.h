/*
 * APBTimer.h
*
 *  Created on: Oct 10, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */


#ifndef APBTIMER_H_
#define APBTIMER_H_

#include "soc_reg.h"
#include <stdint.h>

	typedef struct _apb_timer_t{
		uint32_t CFG_LO;
		uint32_t CFG_HI;
		uint32_t CNT_LO;
		uint32_t CNT_HI;
		uint32_t CMP_LO;
		uint32_t CMP_HI;
		uint32_t START_LO;
		uint32_t START_HI;
		uint32_t RESET_LO;
		uint32_t RESET_HI;
		uint32_t IRQRST_LO;
		uint32_t IRQRST_HI;
	}apb_timer_t;

	#define APB_TMR ((volatile apb_timer_t*)APB_TIMER_BASE)

	#define APB_TMR_CFG_CASC_BIT	31
	#define APB_TMR_CFG_PVAL7_BIT	15
	#define APB_TMR_CFG_PVAL6_BIT	14
	#define APB_TMR_CFG_PVAL5_BIT	13
	#define APB_TMR_CFG_PVAL4_BIT	12
	#define APB_TMR_CFG_PVAL3_BIT	11
	#define APB_TMR_CFG_PVAL2_BIT	10
	#define APB_TMR_CFG_PVAL1_BIT	9
	#define APB_TMR_CFG_PVAL0_BIT	8
	#define APB_TMR_CFG_CCFG_BIT	7
	#define APB_TMR_CFG_PEN_BIT		6
	#define APB_TMR_CFG_ONES_BIT	5
	#define APB_TMR_CFG_MODE_BIT	4
	#define APB_TMR_CFG_IRQEN_BIT	2
	#define APB_TMR_CFG_RST_BIT		1
	#define APB_TMR_CFG_EN_BIT		0

	#define APB_TMR_START_STRT_BIT	0
	#define APB_TMR_RESET_RST_BIT	0
	#define APB_TMR_IRQRST_BIT		0


#endif /* APBTIMER_H_ */
