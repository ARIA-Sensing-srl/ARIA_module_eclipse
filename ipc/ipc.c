/*
 * ipc.c
 *
 *  Created on: Nov 20, 2023
 *      Author: ARIA Sensing
 */

#include "ipc.h"


//define exchange structures
volatile ipc_item_t cv2copr[IPC_NUM_ITEMS] __attribute__((section(".ipc.cv")));
volatile ipc_item_t copr2cv[IPC_NUM_ITEMS] __attribute__((section(".ipc.copr")));



#ifdef CORE_CV
#define txlist cv2copr
#define rxlist copr2cv
#else
#define rxlist cv2copr
#define txlist copr2cv
#endif


int ipc_init(){
#ifdef CORE_CV
	for(int i = 0 ; i< IPC_NUM_ITEMS; i++){
		txlist[i].cmdctl = (IPC_CTL_IDLE << IPC_CMDCTL_CTL_POS);
		rxlist[i].cmdctl = (IPC_CTL_IDLE << IPC_CMDCTL_CTL_POS);
	}
#else
	return IPC_SUCCESS;
#endif
}


int ipc_get_messages(ipc_item_t * pmesg){
	for (int i = 0 ; i< IPC_NUM_ITEMS; i++){
		if (mIPCGetCtl(rxlist[i].cmdctl) == IPC_CTL_BUSY){
			//fill destination
			*pmesg = rxlist[i];
			//free data
			rxlist[i].cmdctl = (IPC_CTL_IDLE << IPC_CMDCTL_CTL_POS);
			return i;
		}
		if (mIPCGetCtl(rxlist[i].cmdctl) == IPC_CTL_BUSY_WAIT){
			//fill destination, do not clear the item
			*pmesg = rxlist[i];
			return i;
		}
	}
	return IPC_NO_MESSAGES;
}


int ipc_send_message(ipc_messagedata_t data, uint32_t command,  bool wait_response){
	//check for free position

	for(int i = 0; i< IPC_NUM_ITEMS; i++){
		if (mIPCGetCtl(txlist[i].cmdctl) == IPC_CTL_IDLE){
			//fill (take care of the order, data before control)
			txlist[i].msgdata = data;
			uint32_t cmdctl = (wait_response ? IPC_CTL_BUSY_WAIT : IPC_CTL_BUSY) <<  IPC_CMDCTL_CTL_POS;
			cmdctl |=  (command << IPC_CMDCTL_CMD_POS);
			txlist[i].cmdctl = cmdctl;
			return i;
		}
	}
	return IPC_NO_SPACE;
}

int ipc_send_response(ipc_messagedata_t data, int index, bool iserror){
	//check if index is in wating state
	if (mIPCGetCtl(rxlist[index].cmdctl) != IPC_CTL_BUSY_WAIT)
		return IPC_INVALID_HANDLER;
	//take care of order, data before control
	rxlist[index].msgdata = data;
	asm volatile("nop");
	uint32_t cmdctl =rxlist[index].cmdctl & IPC_CMD_CTL_CMD_MASK; //preserve cmd field
	cmdctl |= (iserror ? IPC_CTL_READY_ERR : IPC_CTL_READY) << IPC_CMDCTL_CTL_POS;
	rxlist[index].cmdctl = cmdctl;
	return IPC_SUCCESS;
}

int ipc_check_response(ipc_item_t* pitem, int index){
	if (mIPCGetCtl(txlist[index].cmdctl) == IPC_CTL_BUSY_WAIT)
		return IPC_NO_RESPONSE;
	if ((mIPCGetCtl(txlist[index].cmdctl) == IPC_CTL_READY) ||
			(mIPCGetCtl(txlist[index].cmdctl) == IPC_CTL_READY_ERR)){
		*pitem = txlist[index];
		//free handler
		txlist[index].cmdctl = (IPC_CTL_IDLE << IPC_CMDCTL_CTL_POS);
		return IPC_SUCCESS;
	}
	return IPC_INVALID_HANDLER;
}

int ipc_remove(int index){
	uint32_t status = mIPCGetCtl(txlist[index].cmdctl);
	if (status == IPC_CTL_BUSY || status == IPC_CTL_BUSY_WAIT){
		txlist[index].cmdctl = (IPC_CTL_IDLE << IPC_CMDCTL_CTL_POS);
		return IPC_SUCCESS;
	}
	return IPC_INVALID_HANDLER;
}
