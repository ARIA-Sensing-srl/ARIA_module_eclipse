/*
 * hal_evt_ctl.c
 *
 *  Created on: Dec 11, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */



#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "eventctrl.h"
#include "hal_evt_ctl.h"

#if !defined(CORE_COPR) && !defined(CORE_CV)
#error "Undefined processor"
#endif

#if defined(CORE_CV)
#include "interrupts.h"

//define the event table

static struct _evt_callback_table{
	uint8_t evtID;
	void (*pfunc)(void*param);
	void * params;
}evt_callback_table[] = {
	{EVC_EVT_UART_RX_CH_ID, NULL, NULL},
	{EVC_EVT_UART_TX_CH_ID, NULL, NULL},
	{EVC_EVT_UART_RX_ID, NULL, NULL},
	{EVC_EVT_UART_ERR_ID, NULL, NULL},

	{EVC_EVT_SPI0_RX_ID, NULL, NULL},
	{EVC_EVT_SPI0_TX_ID, NULL, NULL},
	{EVC_EVT_SPI0_CMD_ID, NULL, NULL},
	{EVC_EVT_SPI0_EOT_ID, NULL, NULL},

	{EVC_EVT_SPI1_RX_ID, NULL, NULL},
	{EVC_EVT_SPI1_TX_ID, NULL, NULL},
	{EVC_EVT_SPI1_CMD_ID, NULL, NULL},
	{EVC_EVT_SPI1_EOT_ID, NULL, NULL},

	{EVC_EVT_ADV_TIM0_ID, NULL, NULL},
	{EVC_EVT_ADV_TIM1_ID, NULL, NULL},
	{EVC_EVT_ADV_TIM2_ID, NULL, NULL},
	{EVC_EVT_ADV_TIM3_ID, NULL, NULL},
	{EVC_EVT_GPIO_ID, NULL, NULL},
	{EVC_EVT_APBEVT0_ID, NULL, NULL},
	{EVC_EVT_APBEVT1_ID, NULL, NULL},
	{EVC_EVT_APBEVT2_ID, NULL, NULL},
	{EVC_EVT_APBEVT3_ID, NULL, NULL},
	{EVC_EVT_APBEVT4_ID, NULL, NULL},
	{EVC_EVT_APBEVT5_ID, NULL, NULL},
	{EVC_EVT_APBEVT6_ID, NULL, NULL},
	{EVC_EVT_APBEVT7_ID, NULL, NULL},
	{EVC_EVT_SLOW_CLK_ID, NULL, NULL},
};

#define EVT_TBL_ITEMS (sizeof(evt_callback_table)/sizeof(evt_callback_table[0]))

static struct _evt_callback_table * getEvtHandler(uint32_t eventID){
	struct _evt_callback_table * ret = NULL;
	for(int i = 0 ; i< EVT_TBL_ITEMS; i++){
		if (evt_callback_table[i].evtID == eventID){
			ret = &evt_callback_table[i];
			break;
		}
	}
	return ret;
}

int hal_evt_ctl_register(uint32_t eventId, void (*pfunc)(void *), void * param){
	struct _evt_callback_table * phandler;
	phandler = getEvtHandler(eventId);
	if (phandler == NULL)
		return HAL_EVT_CTL_FAIL;
	phandler->params = param;
	phandler->pfunc = pfunc;
	return HAL_EVT_CTL_SUCCESS;
}


int hal_evt_ctl_unmask(uint32_t eventId){
	struct _evt_callback_table * phandler;
	phandler = getEvtHandler(eventId);
	if (phandler == NULL)
		return HAL_EVT_CTL_FAIL;
	mFC_evtUnMask(eventId);
	return HAL_EVT_CTL_SUCCESS;
}

int hal_evt_ctl_mask(uint32_t eventId){
	struct _evt_callback_table * phandler;
	phandler = getEvtHandler(eventId);
	if (phandler == NULL)
		return HAL_EVT_CTL_FAIL;
	mFC_evtMask(eventId);
	return HAL_EVT_CTL_SUCCESS;
}

int hal_evt_ctl_unregister(uint32_t eventId)
{
	struct _evt_callback_table * phandler;
	phandler = getEvtHandler(eventId);
	if (phandler == NULL)
		return HAL_EVT_CTL_FAIL;
	hal_evt_ctl_mask(eventId);
	asm volatile("nop");
	phandler->pfunc = NULL;
	return HAL_EVT_CTL_SUCCESS;
}

//interrupt handler
__attribute__((interrupt ("machine")))
void isr_evt_fifo_valid()
{
	uint32_t fifo = ITC_FIFO;
	struct _evt_callback_table * phandler;
	phandler = getEvtHandler(fifo);
	if (phandler){
		if (phandler->pfunc)
			phandler->pfunc(phandler->params);
	}
};
#endif

int hal_evt_ctl_sw_event(uint8_t swEventNumber){
	//trig a SW event
	if ((swEventNumber < EVC_EVT_SW_EVENT_MIN) || (swEventNumber > EVC_EVT_SW_EVENT_MAX)){
		return HAL_EVT_CTL_FAIL;
	}
	EVC_SW_EVENT = (1 << (swEventNumber-EVC_EVT_SW_EVENT_BASE));
	return HAL_EVT_CTL_SUCCESS;
}
