/**
 * @file uart_app.h
 * @brief UART communication handler
 *
 *  Created on: Dec 11, 2023\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef UART_APP_H_
#define UART_APP_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

/**
 * UART initializer function
 * @param buadrate
 * @return SUCCESS/FAIL
 */
int uart_init(uint32_t buadrate);

/**
 * @brief Send data to UART (partial blocking)
 *
 * This function transfers buffer to UART, if the buffer size is larger than peripheral's DMA handled size, the transaction is split and the function exit\n
 * as soon as the last buffer is transferred to the DMA control\n
 * If transaction is smaller than the DMA handled size, the function transfer the buffer to the DMA and exit (not blocking)\n
 * User must check the transfer completion with uart_isbusy before deallocate the memory or change the buffer content
 * @param buffer
 * @param size
 * @return SUCCESS/FAIL
 */
int uart_send(uint8_t* buffer, size_t size);
/**
 * Enable UART receiver
 * @param enable
 * @return SUCCESS/FAIL
 */
int uart_rx_enable(uint8_t enable);
/**
 * Return the UART transmitter status
 * @return true if transmission is ongoing, false otherwise
 */
bool uart_isbusy();

#define UART_SUCCESS 0
#define UART_FAIL -1
#define UART_TX_BUSY -2

#endif /* UART_APP_H_ */
