/*
 * EventCtrl.h
*
 *  Created on: Oct 10, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef EVENTCTRL_H_
#define EVENTCTRL_H_

#include "soc_reg.h"

	#define EVC_OFFSET				0x00000000
	#define EVC_SW_EVENT_OFFSET		0x00000000
	#define EVC_FC_MASK_OFFSET		0x00000004
	#define EVC_PR_MASK_OFFSET		0x00000044
	#define EVC_ERR_OFFSET			0x00000064
	#define EVC_TIMER_LO_OFFSET		0x00000084
	#define EVC_TIMER_HI_OFFSET		0x00000088

	#define EVC_SW_EVENT 	(*(volatile uint32_t*)(EVC_OFFSET+EVT_GEN_BASE))
	#define EVC_FC_MASK  	((volatile uint32_t*)(EVC_FC_MASK_OFFSET+EVT_GEN_BASE))
	#define EVC_PR_MASK  	((volatile uint32_t*)(EVC_PRMASK_OFFSET+EVT_GEN_BASE))
	#define EVC_ERR  		((volatile uint32_t*)(EVC_ERR_OFFSET+EVT_GEN_BASE))
	#define EVC_TIMER_LO	((volatile uint32_t*)(EVC_TIMER_LO_OFFSET+EVT_GEN_BASE))
	#define EVC_TIMER_HI	((volatile uint32_t*)(EVC_TIMER_HI_OFFSET+EVT_GEN_BASE))


	#define EVC_EVT_UART_RX_CH_ID	0 	//DMA
	#define EVC_EVT_UART_TX_CH_ID	1	//DMA
	#define EVC_EVT_UART_RX_ID		2	//single character
	#define EVC_EVT_UART_ERR_ID		3	//single character
	#define EVC_EVT_SPI0_RX_ID	4
	#define EVC_EVT_SPI0_TX_ID	5
	#define EVC_EVT_SPI0_CMD_ID	6
	#define EVC_EVT_SPI0_EOT_ID	7
	#define EVC_EVT_SPI1_RX_ID	8
	#define EVC_EVT_SPI1_TX_ID	9
	#define EVC_EVT_SPI1_CMD_ID	10
	#define EVC_EVT_SPI1_EOT_ID	11

	#define EVC_EVT_ADV_TIM0_ID	135
	#define EVC_EVT_ADV_TIM1_ID	136
	#define EVC_EVT_ADV_TIM2_ID	137
	#define EVC_EVT_ADV_TIM3_ID	138
	#define EVC_EVT_GPIO_ID		139
	#define EVC_EVT_APBEVT0_ID	160	//generate by writing the SW event register
	#define EVC_EVT_APBEVT1_ID	161
	#define EVC_EVT_APBEVT2_ID	162
	#define EVC_EVT_APBEVT3_ID	163
	#define EVC_EVT_APBEVT4_ID	164
	#define EVC_EVT_APBEVT5_ID	165
	#define EVC_EVT_APBEVT6_ID	166
	#define EVC_EVT_APBEVT7_ID	167
	#define EVC_EVT_SLOW_CLK_ID	168

	#define EVC_EVT_SW_EVENT_BASE EVC_EVT_APBEVT0_ID

	#define EVC_EVT_SW_EVENT_MIN (EVC_EVT_SW_EVENT_BASE)
	#define EVC_EVT_SW_EVENT_MAX (EVC_EVT_SW_EVENT_BASE+7)




	#define mFC_evtUnMask(EvtID) {EVC_FC_MASK[(EvtID)>>5] &= ~(1<<((EvtID) & 0x1F));}
	#define mFC_evtMask(EvtID) {EVC_FC_MASK[(EvtID)>>5] |= (1<<((EvtID) & 0x1F));}

	#define mPR_evtUnMask(EvtID) {EVC_PR_MASK[(EvtID)>>5] &= ~(1<<((EvtID) & 0x1F));}
	#define mPR_evtMask(EvtID) {EVC_PR_MASK[(EvtID)>>5] |= (1<<((EvtID) & 0x1F));}



#endif /* EVENTCTRL_H_ */
