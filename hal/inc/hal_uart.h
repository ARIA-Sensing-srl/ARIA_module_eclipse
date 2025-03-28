/**
 * @file uart.h
 * @brief UART HAL
 *
 *  Created on: Nov 21, 2023\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */


#ifndef UART_H_
#define UART_H_






#include <stdbool.h>
#include <malloc.h>
#include "udma.h"

/**
 * @struct _uart_init_t
 * UART configuration structure
 * @var  _uart_init_t::baudrate
 * baudrate
 * @var  _uart_init_t::pollingMode
 * Set polling mode for reception
 *
 */
typedef struct _uart_init_t{
	uint32_t baudrate;
	uint8_t pollingMode;
}uart_init_t;
/**
 * Configure UART
 * @param pUART pointer to UART handler
 * @param cfg configuration structure
 * @return SUCCESS/FAIL
 */
int 	hal_uart_configure(volatile uDMA_UART_t * pUART, uart_init_t cfg);
/**
 * @brief Transmit data to UART, non blocking
 *
 * The routine start the transmission and exit immediately, the data aren't copied to any internal buffer
 * @param pUART pointer to UART handler
 * @param pbuffer data buffer, the buffer mustn't be modificed until transmission is complete
 * @param size bytes to transfer
 * @return SUCCESS/FAIL
 */
int 	hal_uart_send(volatile uDMA_UART_t * pUART, uint8_t* pbuffer, size_t size);
/**
 * Check if there are valid character into the receiver FIFO
 * @param pUART pointer to peripheral handler
 * @param pval pointer to destination memory
 * @return SUCCESS/HAL_UART_NO_CHARACTERS
 */
int 	hal_uart_poll(volatile uDMA_UART_t * pUART, uint8_t* pval);
/**
 * Check if last transmission phase is complete
 * @param pUART
 * @return true if transmission complete, false if busy
 */
bool 	hal_uart_send_complete(volatile uDMA_UART_t * pUART);
/**
 * Enable receiver interrupt
 * @param pUART
 * @param enable
 * @return SUCCESS/FAIL
 */
int     hal_uart_rxint(volatile uDMA_UART_t * pUART, bool enable);
/**
 * Enable receiver error interrupt
 * @param pUART
 * @param enable
 * @return SUCCESS/FAIL
 */
int     hal_uart_errint(volatile uDMA_UART_t * pUART, bool enable);
/**
 * De-initialize and disable UART
 * @param pUART
 * @return SUCCESS/FAIL
 */
int 	hal_uart_deinit(volatile uDMA_UART_t * pUART);

#define HAL_UART_SUCCESS 0
#define HAL_UART_FAIL -1
#define HAL_UART_NO_CHARACTERS -2
#define HAL_UART_INVALID_CHANNEL -3
#define HAL_UART_TX_STILL_ONGOING -4





#endif /* UART_H_ */
