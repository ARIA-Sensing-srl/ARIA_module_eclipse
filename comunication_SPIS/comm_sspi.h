/**
 * @file comm_sspi.h
 * @brief This interface perform IO between an external SPI master and memory mapped buffer
 *
 * This code handles communication between an external SPI master and the module, performing data transfer and communication handshaking
 *
 *
 *  Created on: May 31, 2024\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef COMM_SSPI_H_
#define COMM_SSPI_H_

#include "comm_cb_types.h"
#include "radar_comm_def.h"

/**
 * Peripheral and memory initialization procedure
 * @return SUCCESS/FAIL
 */
int comm_sspi_init();
/**
 * Communication handler task, call it periodically
 * @return SUCCESS/FAIL
 */
int comm_sspi_task();
/**
 * Check if message is available and return command and payload
 * @param pcmd pointer to destination command location
 * @param ppayload pointer to destination payload location
 * @return SUCCESS/NOMESSAGE
 */
int comm_sspi_getmessage(radar_command* pcmd, radar_command_payload* ppayload);
/**
 * Send a buffer to the SPI
 * @param pbuf	pointer to data buffer
 * @param size  buffer size
 * @param isstart mark the beginning of a new message
 * @param isend mark the end of the message
 * @param encode ingored by the SPI
 * @return
 */
int comm_sspi_send_stream(uint8_t * pbuf, int size, bool isstart, bool isend, bool encode);
/**
 * Send a message stored into comm_msgbuffer_t
 * @param msgbuf comm_msgbuffer_t, requested with comm_getbuffer function
 * @return SUCCESS/FAIL
 */
int comm_sspi_send(comm_msgbuffer_t msgbuf);
/**
 * Require access to internal transmission memory buffer
 * @param size required size
 * @param pbuffer pointer to message buffer
 * @return SUCCESS/FAIL
 */
int comm_sspi_getbuffer(int size, comm_msgbuffer_t* pbuffer);



#define COMM_SSPI_SUCCESS 0
#define COMM_SSPI_ERROR -1
#define COMM_SSPI_NO_MEMORY -2
#define COMM_SSPI_NO_MESSAGE -5

#endif /* COMM_SSPI_H_ */
