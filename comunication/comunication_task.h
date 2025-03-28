/**
 * @file comunication_task.h
 * @brief UART channel communication handler
 *
 * This routines perform message transmission/reception and provide support for protocol encoding\n
 *
 *  Created on: 2 set 2020\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef COMM_COMUNICATION_TASK_H_
#define COMM_COMUNICATION_TASK_H_


#include "stdint.h"
#include "radar_comm_def.h"
#include "stdbool.h"
#include "comm_cb_types.h"

#define COMM_TASK_RX_MASK 0xFF
#define COMM_TASK_RX_SIZE (COMM_TASK_RX_MASK+1)

#define COMM_TASK_TX_SIZE 4096
#define COMM_TASK_RX_TO_ms 1000
#define COMM_STREAM_MAX_CHUNK_SIZE (COMM_TASK_TX_SIZE/2)


/**
 * Communication handler task, call this function periodically
 */
void comm_task();
/**
 * Check if there'are incoming message from UART
 * @param pcmd pointer to command
 * @param ppayload  pointer to payload
 * @return SUCCESS/NOMESSAGE
 */
int comm_getmessage(radar_command* pcmd, radar_command_payload* ppayload);
/**
 * Require access to internal transmission memory buffer
 * @param size required size
 * @param pbuffer pointer to message buffer
 * @return SUCCESS/FAIL
 */
int comm_getbuffer(int size, comm_msgbuffer_t* pbuffer);
/**
 * Send a message stored into comm_msgbuffer_t
 * @param buffer comm_msgbuffer_t, requested with comm_getbuffer function
 * @return SUCCESS/FAIL
 */
int comm_send(comm_msgbuffer_t buffer);

/**
 * Send a buffer to the UART
 * @param pbuf	pointer to data buffer
 * @param size  buffer size
 * @param isstart if set, the function prepend start byte before the message
 * @param isend if set, the function send the stop byte after the last buffer byte
 * @param encode if set, buffer is byte-stuffed encoded before sending
 * @return
 */
int comm_send_stream(uint8_t * pbuf, int size, bool isstart, bool isend, bool encode);

#define COMM_SUCCESS 0
#define COMM_FAIL -1
#define COMM_NO_MEMORY -2
#define COMM_NO_MESSAGE -5

#endif /* COMM_COMUNICATION_TASK_H_ */
