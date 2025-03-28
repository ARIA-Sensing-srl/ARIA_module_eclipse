/*
 * uart.c
 *
 *  Created on: Nov 22, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */



#include "stdint.h"


	#include <stdbool.h>
	#include <stdint.h>

	#include "udma.h"


	#include "hal_uart.h"
	#include "hal_clkmgr.h"


	int hal_uart_configure(volatile uDMA_UART_t * pUART, uart_init_t config)
	{

		uint32_t perclk;
		//compute peripheral clock
		//perclk = hal_bbpll_getcorefrequency_Mhz(); //return core in MHz
		//uint32_t divratio = mHalRccGetCpuDivisorFactor(hal_rcc_get_cpuclkdiv()) *  mHalRccGetPerphDivisorFactor(hal_rcc_get_perclkdiv());
		//perclk *= 1000000UL;

		perclk  = hal_clkmgh_get_peripheral_freq();

		//cleanup setup register
		pUART->SETUP |= ( 1 << UDMA_UART_SETUP_CLEANFIFO_BIT);
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		pUART->SETUP = 0;
		while(pUART->SETUP){
			asm volatile("nop");
		}
		uint32_t baud = perclk/config.baudrate;

		pUART->SETUP = 	(baud << UDMA_UART_SETUP_CLKDIV0_BIT) | \
							(1 << UDMA_UART_SETUP_TXEN_BIT) | \
							(3 << UDMA_UART_SETUP_BITLGTH0_BIT) | \
							(1 << UDMA_UART_SETUP_RXEN_BIT) | \
							((config.pollingMode & 0x1) << UDMA_UART_SETUP_POLLINGEN_BIT);

		volatile uint8_t dummy;
		while(hal_uart_poll(pUART, &dummy) == HAL_UART_SUCCESS); //flush buffer
		return HAL_UART_SUCCESS;
	}



	int hal_uart_send(volatile uDMA_UART_t * pUART, uint8_t* pbuffer, size_t size)
	{
		if (!hal_uart_send_complete(pUART))
			return HAL_UART_TX_STILL_ONGOING;
		//transfer data
		if (size > UDMA_UART_MAX_SIZE)
			size = UDMA_UART_MAX_SIZE;
		pUART->TX_SADDR = (uint32_t)pbuffer;
		pUART->TX_SIZE = size;
		pUART->TX_CFG = (1<<UDMA_UART_TX_CFG_EN_BIT);
		return size; //return size transferred
	}

	int hal_uart_rxint(volatile uDMA_UART_t * pUART, bool enable){
		if (enable){
			pUART->IRQ_EN |= (1<<UDMA_UART_IRQEN_RX_BIT);
		} else {
			pUART->IRQ_EN &= ~(1<<UDMA_UART_IRQEN_RX_BIT);
		}
		return HAL_UART_SUCCESS;
	}

	int hal_uart_errint(volatile uDMA_UART_t * pUART, bool enable){
		if (enable){
			pUART->IRQ_EN |= (1<<UDMA_UART_IRQEN_ERROR_BIT);
		} else {
			pUART->IRQ_EN &= ~(1<<UDMA_UART_IRQEN_ERROR_BIT);
		}
		return HAL_UART_SUCCESS;
	}


	int 	hal_uart_deinit(volatile uDMA_UART_t * pUART){
		pUART->IRQ_EN = 0;
		pUART->RX_CFG |= (1 << UDMA_UART_RX_CFG_CLR_BIT);
		pUART->TX_CFG |= (1 << UDMA_UART_TX_CFG_CLR_BIT);
		pUART->SETUP &= ~((1 << UDMA_UART_SETUP_TXEN_BIT) | (1 << UDMA_UART_SETUP_TXEN_BIT));
		pUART->SETUP |= ( 1 << UDMA_UART_SETUP_CLEANFIFO_BIT);
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		pUART->RX_CFG = 0;
		pUART->TX_CFG = 0;

		pUART->SETUP = 0;
	}


	int hal_uart_poll(volatile  uDMA_UART_t * pUART, uint8_t* pval)
	{

		if (pUART->VALID){
			volatile uint32_t curChar;
			curChar = pUART->DATA;
			*pval = curChar;
			return HAL_UART_SUCCESS;
		}
		return HAL_UART_NO_CHARACTERS;
	}


	bool hal_uart_send_complete(volatile uDMA_UART_t * pUART)
	{
		if (pUART->TX_CFG & (1<<UDMA_UART_TX_CFG_EN_BIT))
			return false;
		return true;
	}

