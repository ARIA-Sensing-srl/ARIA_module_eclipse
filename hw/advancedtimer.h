/*
 * AdvancedTimer.h
*
 *  Created on: Oct 10, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef ADVANCEDTIMER_H_
#define ADVANCEDTIMER_H_

  #include "soc_reg.h"

  #include <stdint.h>
	#define ADV_TMR0_OFFSET 		0x00000000
	#define ADV_TMR1_OFFSET 		0x00000040
	#define ADV_TMR2_OFFSET 		0x00000080
	#define ADV_TMR3_OFFSET 		0x000000C0
	#define ADV_TMR_EVENT_OFFSET	0x00000100
	#define ADV_TMR_CG_OFFSET		0x00000104


	typedef volatile struct _adv_tmr_t{
		uint32_t CMD;
		uint32_t CONFIG;
		uint32_t CNTTHR;
		uint32_t TH_CH[4];
		uint32_t COUNTER;
	}adv_tmr_t;

	#define ADV_TMR0	((volatile adv_tmr_t*) (ADV_TIMER_BASE + ADV_TMR0_OFFSET))
	#define ADV_TMR1	((volatile adv_tmr_t*) (ADV_TIMER_BASE + ADV_TMR1_OFFSET))
	#define ADV_TMR2	((volatile adv_tmr_t*) (ADV_TIMER_BASE + ADV_TMR2_OFFSET))
	#define ADV_TMR3	((volatile adv_tmr_t*) (ADV_TIMER_BASE + ADV_TMR3_OFFSET))
	#define ADV_TMR_EVNT (*(volatile uint32_t*)(ADV_TMR_EVENT_OFFSET+ADV_TIMER_BASE))
	#define ADV_TMR_CG (*(volatile uint32_t*)(ADV_TMR_CG_OFFSET+ADV_TIMER_BASE))

	#define ADV_TMR_CMD_ARM_POS				4
	#define ADV_TMR_CMD_RST_POS				3
	#define ADV_TMR_CMD_UPDATE_POS			2
	#define ADV_TMR_CMD_STOP_POS			1
	#define ADV_TMR_CMD_START_POS			0

	#define ADV_TMR_CONFIG_PRESC_POS		16
	#define ADV_TMR_CONFIG_PRESC_MASK		(0xFF << ADV_TMR_CONFIG_PRESC_POS)


	#define ADV_TMR_CONFIG_UPDWN_POS		12 //0 alternate cnt down and up, 1 up count and reset to 0
	#define ADV_TMR_CONFIG_CLKSEL_POS				11 //0 master, 1 slowclock
	#define ADV_TMR_CONFIG_MODE_POS			8
	#define ADV_TMR_CONFIG_MODE_MASK		(0x7 << ADV_TMR_CONFIG_MODE_POS)


	#define ADV_TMR_CONFIG_INSEL_POS		0
	#define ADV_TMR_CONFIG_INSEL_MASK		(0xFF << ADV_TMR_CONFIG_INSEL_POS)


	#define ADV_TMR_CNTTHR_LO_POS		0
	#define ADV_TMR_CNTTHR_HI_POS		16

	#define ADV_TMR_CNTTHR_LO_MASK		(0xFFFF << ADV_TMR_CNTTHR_LO_POS)
	#define ADV_TMR_CNTTHR_HI_MASK		(0xFFFF << ADV_TMR_CNTTHR_HI_POS)

	#define ADV_TMR_TH_CH_THR_POS			0
	#define ADV_TMR_TH_CH_THR_MASK			(0xFFFF << ADV_TMR_TH_CH_THR_POS)

	#define ADV_TMR_TH_CH_MODE_POS			16
	#define ADV_TMR_TH_CH_MODE_MASK			(0x7 << ADV_TMR_TH_CH_MODE_POS)

	#define ADV_TMR_EVNT_EVT0SRC_CFG_POS			0
	#define ADV_TMR_EVNT_EVT1SRC_CFG_POS			4
	#define ADV_TMR_EVNT_EVT2SRC_CFG_POS			8
	#define ADV_TMR_EVNT_EVT3SRC_CFG_POS			12

	#define ADV_TMR_EVT0EN_POS				16
	#define ADV_TMR_EVT1EN_POS				17
	#define ADV_TMR_EVT2EN_POS				18
	#define ADV_TMR_EVT3EN_POS				19

	#define ADV_TMR_EVNT_EVT0SRC_CFG_MASK		(0xF<<ADV_TMR_EVNT_EVT0SRC_CFG_POS)
	#define ADV_TMR_EVNT_EVT1SRC_CFG_MASK		(0xF<<ADV_TMR_EVNT_EVT1SRC_CFG_POS)
	#define ADV_TMR_EVNT_EVT2SRC_CFG_MASK		(0xF<<ADV_TMR_EVNT_EVT2SRC_CFG_POS)
	#define ADV_TMR_EVNT_EVT3SRC_CFG_MASK		(0xF<<ADV_TMR_EVNT_EVT3SRC_CFG_POS)

	#define ADV_TMR_CG_TMR0_POS				0
	#define ADV_TMR_CG_TMR1_POS				1
	#define ADV_TMR_CG_TMR2_POS				2
	#define ADV_TMR_CG_TMR3_POS				3

	//macro to return register content for specified event and val
        #define mADV_TMR_EVNT_EVTx_SRC(ID, val) ((val) << (4*ID + ADV_TMR_EVNT_EVT0SRC_CFG_POS))
        #define mADV_TMR_EVNT_EVTx_MASK(ID) (mADV_TMR_EVNT_EVTx_SRC(ID, 0xF))
        #define mADV_TMR_EVNT_ENx(ID) (1 << (ID+ADV_TMR_EVT0EN_POS))

        #define mADV_TMR_CG_TMRx(ID) (1 << (ID+ADV_TMR_CG_TMR0_POS))


#endif /* ADVANCEDTIMER_H_ */
