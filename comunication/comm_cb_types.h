/*
 * comm_cb_types.c
 *
 *  Created on: Jan 31, 2024
 *      Author: ARIA Sensing
 */

#ifndef COMM_CB_TYPES_H_
#define COMM_CB_TYPES_H_

#include "hydrogen_UD_v1.h"
#include "reconstruction_2d.h"

#define COMM_CB_SEND_REGULAR		0	//the callback update the value inside the payload, calleer must send a regular formatted response
#define COMM_CB_SENT_CUSTOM 		1	//handler has already sent/or taken care of the communication, no further action are needed
#define COMM_CB_IGNORE				COMM_CB_SENT_CUSTOM
#define COMM_CB_ERR_IO				-1	//something wrong happened during the execution of the command
#define COMM_CB_ERR_COMUNICATION	-2	//something wrong happened during custom response


typedef struct _comm_msgbuffer_t{
	uint8_t * buffer;
	int size;
}comm_msgbuffer_t;


typedef struct _comm_cb_args_t{
	int interfaceID;
	HydrUDriver_t * pdrv;
	recon_handler_t * prec;
	HydrUDrv_acqHandler_t * pacqHanlders;
	int numAcqHandlers;
}comm_cb_args_t;

#endif /* COMM_CB_TYPES_H_ */
