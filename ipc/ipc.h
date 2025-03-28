/*
 * ipc.c
 *
 *  Created on: Nov 20, 2023
 *      Author: ARIA Sensing
 */

#ifndef IPC_H_
#define IPC_H_

#include "stdbool.h"
#include "stdint.h"

#define IPC_NUM_ITEMS 16


#if !defined(CORE_CV) && !defined(CORE_COPR)
#error "undefined core";
#endif

/*IPC comunication headers, first word encompass command and ctrl, second word is the data/datapointer
usage depends on command type
Every message is one-way and the behavior of sendere/receiver depends on request code
*/

typedef volatile union _ipc_messagedata_t{
	void * pdata;
	uint32_t u32;
	int32_t i32;
}ipc_messagedata_t;

typedef struct _ipc_item_t{
	uint32_t cmdctl;
	ipc_messagedata_t msgdata;
}ipc_item_t;


//mesage structure
#define IPC_CMDCTL_CMD_POS 0
#define IPC_CMDCTL_CTL_POS 16

//masks
#define IPC_CMD_CTL_CMD_MASK (0xFFFF << IPC_CMDCTL_CMD_POS)
#define IPC_CMD_CTL_CTL_MASK (0x7 << IPC_CMDCTL_CTL_POS)


//controls code
#define IPC_CTL_IDLE 		0	//no message, item is free
#define IPC_CTL_BUSY 		1	//message inside, no response required
#define IPC_CTL_BUSY_WAIT 	2	//sender wait for response
#define IPC_CTL_READY 		3	//responder accept and filled data with response
#define IPC_CTL_READY_ERR 	4	//responder reject request, data section if filled with error code


#define IPC_INVALID_HANDLER -1
#define IPC_NO_RESPONSE -2
#define IPC_NO_MESSAGES -255
#define IPC_NO_SPACE -128
#define IPC_SUCCESS 0

//aux macros for parsing cmdctl field
#define mIPCGetCtl(x) (((x) & IPC_CMD_CTL_CTL_MASK) >> IPC_CMDCTL_CTL_POS)
#define mIPCGetCmd(x) (((x) & IPC_CMD_CTL_CMD_MASK) >> IPC_CMDCTL_CMD_POS)

/*
 * Copy a new message into the handler, and return the index if a response handling is required
 * The internal buffer is released if the message type is IPC_CTL_BUSY, otherwise the buffer is locked until a response is provided
 * */
int ipc_get_messages(ipc_item_t *);

/*
 * Send a new message and return the internal handler to manage the response (is wait_response is set)
 * */
int ipc_send_message(ipc_messagedata_t, uint32_t command,  bool wait_response);

/*
 * Send a response message, the index must be valid (returned from ipc_get_messages)
 * */
int ipc_send_response(ipc_messagedata_t, int index, bool iserror);


/*
 * Check if a response is ready, called by the sender to check the availability of a response.
 *
 * */
int ipc_check_response(ipc_item_t*, int index);


int ipc_init();


//remove pending message from message list
int ipc_remove(int index);




#endif /* IPC_H_ */
