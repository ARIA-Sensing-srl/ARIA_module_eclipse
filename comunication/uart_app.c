/*
 * uart_app.c
 *
 *  Created on: Dec 11, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#include <uart_app.h>
#include "hal_uart.h"
#include "hal_evt_ctl.h"
#include "hal_udma.h"
#include "udma.h"
#include "stddef.h"

//** DEFINES START **//
#define UART_CHANNEL UDMA_UART0
//** DEFINES END **//

//** MACRO START **//

//** MACRO END **//

//** TYPES START **//

//** TYPES END **//

//** PRIVATE VARIABLES START **//
static volatile uint8_t uart_enable = 1;

//** PRIVATE VARIABLES END **//

//** PUBLIC VARIABLES START **//

//** PUBLIC VARIABLES END **//

//** PRIVATE FUNCTIONS START **//
static void priv_rxfunc(void * param);
static void priv_errfunc(void * param);

//** PRIVATE FUNCTIONS END **//

//** PUBLIC FUNCTIONS START **//
extern void CDC_cb_RxData(uint8_t * pbuf, uint16_t Len);
//** PUBLIC FUNCTIONS END **//




int uart_init(uint32_t buadrate){
	//startup
	hal_udma_peripheral_reset(UDMA_UART0_PERID);
	hal_udma_peripheral_ck_enable(UDMA_UART0_PERID);

	//setup UART
	uart_init_t config;
	config.baudrate = buadrate;
	config.pollingMode = 0;
	hal_uart_configure(UART_CHANNEL, config);

	hal_evt_ctl_register(EVC_EVT_UART_RX_ID, priv_rxfunc, NULL);
	hal_evt_ctl_register(EVC_EVT_UART_ERR_ID, priv_errfunc, NULL);
	hal_evt_ctl_unmask(EVC_EVT_UART_RX_ID);
	hal_evt_ctl_unmask(EVC_EVT_UART_ERR_ID);

	//interrupt enable
	hal_uart_rxint(UART_CHANNEL, true);
	hal_uart_errint(UART_CHANNEL, true);
	return UART_SUCCESS;

}

int uart_deinit(){
	return hal_uart_deinit(UART_CHANNEL);
}
int uart_send(uint8_t* buffer, size_t size){
	if (uart_isbusy()){
		return UART_TX_BUSY;
	}
	int residual = size;
	do{
		int transferred = hal_uart_send(UART_CHANNEL, buffer, size);
		if (transferred < 0)
			return UART_FAIL;
		residual -= transferred;
		if (residual){
			while(!hal_uart_send_complete(UART_CHANNEL)){
				asm volatile("nop");
			}
		}
	}while(residual);
	return UART_SUCCESS;
}

int uart_rx_enable(uint8_t en){
	uart_enable  = en;
	return UART_SUCCESS;
}

bool uart_isbusy(){
	return !hal_uart_send_complete(UART_CHANNEL);
}

static void priv_rxfunc(void * param __attribute__((unused))){
	//callback from event controller
	while(UART_CHANNEL->VALID){
		uint32_t uart_rx_char = UART_CHANNEL->DATA; //use ui32 type to ensure WORD type access
		if (uart_enable)
			CDC_cb_RxData(&uart_rx_char, 1);
	};
}

static void priv_errfunc(void * param __attribute__((unused))){
	return;
}


