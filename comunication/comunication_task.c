/*
 * comunication_task.c
 *
 *  Created on: 2 set 2020
 *      Author: ARIA Sensing
 */

#include "comunication_task.h"
#include "radarcommands.h"
#include "stdbool.h"
#include "version.h"
#include "msgEncoderDecored.h"
#include "stddef.h"
#include "string.h"
#include "main.h"
#include "uart_app.h"

//** DEFINES START **//
#define PROTO_START_CHAR 	0xFF
#define PROTO_STOP_CHAR 0x00
#define PROTO_CHECK_CHAR 0x80
#define PROTO_ALTERNATE_START_CHAR 0xFE
#define PROTO_ALTERNATE_STOP_CHAR 0x01
#define PROTO_ALTERNATE_CHECK_CHAR 0x81

#define MAX_QUEUE_LGTH_MASK 15 //binary mask
#define MAX_QUEUE_LGTH (MAX_QUEUE_LGTH_MASK+1)
//** DEFINES END **//

//** MACRO START **//
#define interpret_float(pptr, pval, pend) priv_interpret_void(pptr, (void*)(pval), pend, sizeof(float))
#define interpret_uint8(pptr, pval, pend) priv_interpret_void(pptr, (void*)(pval), pend, sizeof(uint8_t))
#define interpret_uint16(pptr, pval, pend) priv_interpret_void(pptr, (void*)(pval), pend, sizeof(uint16_t))
#define interpret_uint32(pptr, pval, pend) priv_interpret_void(pptr, (void*)(pval), pend, sizeof(uint32_t))

//** MACRO END **//

//** TYPES START **//

//** TYPES END **//

//** PRIVATE VARIABLES START **//
static uint8_t TxData[COMM_TASK_TX_SIZE+4] /*__attribute__((section(".sdram")))*/ __attribute__((aligned(4)));
//circular buffer for queue
static uint8_t queueLgthWr=0;
static uint8_t queueLgthRd=0;
static struct _messQueue{
	radar_command cmd;
	//radar_response resp;
	radar_command_payload payload;
}messQueue[MAX_QUEUE_LGTH];

/*static struct _rxBuffer{
	uint8_t data[COMM_TASK_RX_SIZE];
	uint16_t size;
	uint32_t toCnt;
}rxBuffer;*/
static struct _rxCircBuffer{
	uint8_t data[COMM_TASK_RX_SIZE];
	uint16_t rdi;
	uint16_t wri;
	uint32_t toCnt;
}rxBufferCirc;
static uint32_t lastExeTick = 0;
volatile uint8_t rxflag = 0;

//** PRIVATE VARIABLES END **//

//** PUBLIC VARIABLES START **//

//** PUBLIC VARIABLES END **//

//** PRIVATE FUNCTIONS START **//

static int priv_interpret_void(uint8_t ** ptr, void * value, uint8_t * array_end, size_t size);
static int priv_decode_rx_data();
static int priv_get_rx_space();
static int priv_push_rx_data(uint8_t );
static int priv_get_rx_dataavail();

//** PRIVATE FUNCTIONS END **//

//** PUBLIC FUNCTIONS START **//

//** PUBLIC FUNCTIONS END **//


static int priv_interpret_void(uint8_t ** ptr, void * value, uint8_t * array_end, size_t size){
	if (*ptr+size > array_end)
		return 1;

	uint8_t * pvalU8 = (uint8_t*)value;
	while(size--){
		*(pvalU8++) = *((*ptr)++);
	}
	return 0;
}

static int priv_get_rx_dataavail(){
	return ((rxBufferCirc.wri-rxBufferCirc.rdi) & COMM_TASK_RX_MASK);
}

static int priv_get_rx_space(){
	return ((COMM_TASK_RX_SIZE-1) - priv_get_rx_dataavail());
}
static int priv_push_rx_data(uint8_t data){
	uint16_t wriNext = (rxBufferCirc.wri+1) & COMM_TASK_RX_MASK;
	if (wriNext == rxBufferCirc.rdi){
		return -1;
	}
	rxBufferCirc.data[rxBufferCirc.wri] = data;
	rxBufferCirc.wri = wriNext;
}

void CDC_cb_RxData(uint8_t * pbuf, uint16_t Len){
	//check residual space
	int residual = priv_get_rx_space();

	if (residual >= Len){
		//trasfer all
		for(int i = 0 ; i< Len ; i++){
			priv_push_rx_data(pbuf[i]);
		}
		rxflag = 1;
	}else if (residual != 0){
		for(int i = 0 ; i< residual ; i++){
			priv_push_rx_data(pbuf[i]);
		}
		rxflag = 1;
	}
	//ignore if no data space is available
}

int comm_getmessage(radar_command* pcmd, radar_command_payload* ppayload){
	//check if circular buffer is empty if no, update
	if (queueLgthRd != queueLgthWr){
		//messages pending
		*pcmd = messQueue[queueLgthRd].cmd;
		//answer = messQueue[queueLgthRd].resp;
		*ppayload = messQueue[queueLgthRd].payload;
		queueLgthRd = (queueLgthRd+1) & MAX_QUEUE_LGTH_MASK;
	}else{
		return COMM_NO_MESSAGE;
	}
	return COMM_SUCCESS;
}

static int priv_decode_rx_data()
{

	radar_command tempCommand;
	//radar_response tempResponse;
	radar_command_payload tempPayload;

	tempCommand = RADARCOMM_UNKNOWN_COMMAND;
	//tempResponse  = RADARRESP_NOANSW;

	uint8_t started = 0x00;
	int dataAvailable = priv_get_rx_dataavail();

	//minimum command is start, command, stop, a minimum of 3 bytes is required
	if (dataAvailable < 3)
		return -1;

	uint16_t rdIndex = rxBufferCirc.rdi; //data are accessed without altering rd index, in case message is incomplete

	uint8_t payloadDecoded[128]; //maximum uncompressed message lgth
	uint8_t payloadCnt=0;
	bool findedStart=false;
	bool findedStop=false;
	bool checkChar=false;
	uint8_t crc=0;

	for(int i=0; i < dataAvailable; i++){
		uint8_t curData;
		curData = rxBufferCirc.data[rdIndex];
		rdIndex = (rdIndex+1) & COMM_TASK_RX_MASK;
		if (!findedStart){
			if (curData == PROTO_START_CHAR)
				findedStart = true;
		}else{
			//start already finded
			if (curData == PROTO_CHECK_CHAR){
				checkChar = true;
				continue;
			}else{
				uint8_t dataDecoded;
				if (checkChar){
					checkChar = false;
					switch(curData){
						case PROTO_ALTERNATE_CHECK_CHAR:
							dataDecoded=PROTO_CHECK_CHAR;
							break;
						case PROTO_ALTERNATE_START_CHAR:
							dataDecoded=PROTO_START_CHAR;
							break;
						case PROTO_ALTERNATE_STOP_CHAR:
							dataDecoded=PROTO_STOP_CHAR;
							break;
						default:
							return -1; //coding error
					}
				}else{
					if (curData == PROTO_STOP_CHAR){
						//stop char finded
						findedStop = true;
						break; //
					}
					dataDecoded = curData;
				}
				payloadDecoded[payloadCnt]=dataDecoded;
				payloadCnt++;
				//update crc
#ifdef CRC8_ENABLE
				crc8(&crc, dataDecoded);
#endif
				if (payloadCnt == 128)
					break;
			}
		}
	}

	if (!findedStop || crc!=0){
		tempCommand = RADARCOMM_UNKNOWN_COMMAND;
		return -1;

	}

	//command correctly parsed, update circularBuffer and proceed with decoding
	rxBufferCirc.rdi = rdIndex;


	// Parse command
	uint8_t * pdata = payloadDecoded;
	uint8_t * pend = &payloadDecoded[payloadCnt];

	started=0x01;
	const struct _RadarCommandItem * curCmdItem=0;
	while (pdata < pend)
	{

		if (started==0x02)
		{
			switch (curCmdItem->payloadType){
				case PT_NO:
					break;
				case PT_U8_SR:
				case PT_U8:
					if (interpret_uint8(&pdata,&(tempPayload.data_uint8),pend)!=0)
					{
						tempCommand = RADARCOMM_UNKNOWN_COMMAND;
					}
					break;
				case PT_U16_SR:
				case PT_U16:
					if (interpret_uint16(&pdata,&(tempPayload.data_uint16),pend)!=0)
					{
						tempCommand = RADARCOMM_UNKNOWN_COMMAND;
					}
					break;
				case PT_U32_SR:
				case PT_U32:
					if (interpret_uint32(&pdata,&(tempPayload.data_uint32),pend)!=0)
					{
						tempCommand = RADARCOMM_UNKNOWN_COMMAND;
					}
					break;
				case PT_FLOAT_SR:
				case PT_FLOAT:
					if (interpret_float(&pdata,&(tempPayload.data_float),pend)!=0)
					{
						tempCommand = RADARCOMM_UNKNOWN_COMMAND;
					}
					break;
				case PT_STRUCT_SR:
				case PT_STRUCT:
					{
						uint8_t size=0;
						while((pdata != pend) && (size < sizeof(tempPayload.data_struct.buffer))){
							tempPayload.data_struct.buffer[size++]=*pdata++;
						}
						if (pdata != pend){
							//means message is bigger than buffer. Unable to handle.
							tempCommand = RADARCOMM_UNKNOWN_COMMAND;
							break;
						}
						tempPayload.data_struct.size=size;
					}
					break;
				default:
					tempCommand = RADARCOMM_UNKNOWN_COMMAND; //discard message with undefined payload coding
					break;
			}
			started = 0x03;
			goto exit;
		}

		if (started==0x01)
		{
			uint8_t command_byte = *pdata;

			tempCommand = RADARCOMM_UNKNOWN_COMMAND;
			//tempResponse  = RADARRESP_UNKNOWN_COMMAND;
			//search in command list
			for(int i=0; i < RDR_CMD_LIST_SIZE; i++){
				if (RadarCommandItem[i].cmdCode == command_byte){
					tempCommand = RadarCommandItem[i].cmd;
					//tempResponse = RadarCommandItem[i].rsp;
					curCmdItem = &RadarCommandItem[i];
					break;
				}
			}
			if ((tempCommand == RADARCOMM_UNKNOWN_COMMAND) /*&& (tempResponse == RADARRESP_UNKNOWN_COMMAND)*/)
				return -1;

			tempPayload.data_uint32=0;
			started = 0x02;
		}
		pdata++;
	}

exit:
	{
		//add to queue
		int freeSpace = (((queueLgthRd - queueLgthWr) & MAX_QUEUE_LGTH_MASK))-1;
		if (freeSpace){
			//freeIndex = queueLgthWr;
			messQueue[queueLgthWr].cmd = tempCommand;
			messQueue[queueLgthWr].payload = tempPayload;
			//messQueue[queueLgthWr].resp = tempResponse;
			queueLgthWr = (queueLgthWr+1) & MAX_QUEUE_LGTH_MASK; //update
		}
	}

	return 0;
}


void comm_task(){
	volatile uint32_t curTimeTick = HAL_getSysTick();
	uint32_t elapseTime = curTimeTick - lastExeTick;
	if (elapseTime < 2)
		return;
	//execute periodically (every 10 ms)
	lastExeTick = curTimeTick;

	//check timeout
	if (priv_get_rx_dataavail()){
		if (rxflag){
			rxflag = 0;
			rxBufferCirc.toCnt = 0;
		}
		else{
			rxBufferCirc.toCnt += elapseTime;
		}
		if (rxBufferCirc.toCnt > COMM_TASK_RX_TO_ms){
			//flush data
			uart_rx_enable(0);
			//execute critical section
			rxBufferCirc.rdi = rxBufferCirc.wri;
			rxBufferCirc.toCnt = 0;
			uart_rx_enable(1);
		} else {
			//process data, continue until ret != 0 & no data are available
			int ret;
			do{
				ret = priv_decode_rx_data();
			}while((ret == 0) & priv_get_rx_dataavail());
		}
	}
}


int comm_getbuffer(int size, comm_msgbuffer_t* pbuf){
	if (size > (COMM_TASK_TX_SIZE/2)){
		return COMM_NO_MEMORY;
	}
	pbuf->buffer = &TxData[COMM_TASK_TX_SIZE/2];
	pbuf->size = size;
	return COMM_SUCCESS;
}
int comm_send(comm_msgbuffer_t msgbuf){
	if (msgbuf.size > (COMM_TASK_TX_SIZE/2)){
		return COMM_NO_MEMORY;
	}
	return comm_send_stream(msgbuf.buffer, msgbuf.size, true, true, true);
}
int comm_send_stream(uint8_t * pbuf, int size, bool isstart, bool isend, bool encode){
	if (size > COMM_TASK_TX_SIZE/2)
		return COMM_NO_MEMORY;

	while(uart_isbusy());

	uint8_t *ptBuf = TxData;
	if (isstart)
		*ptBuf++ = PROTO_START_CHAR;
	if (encode){
		//code_void(&ptBuf, (void*)pbuf, &TxData[sizeof(TxData)-1], size);
		int encodedSize = MSGEncode(pbuf, ptBuf, size);
		ptBuf += encodedSize;
	}else{
		memcpy((void*)ptBuf, (const void*)pbuf, size);
		ptBuf += size;
	}
	if (isend)
		*ptBuf++ = PROTO_STOP_CHAR;
	uint32_t txfrSize = (uint32_t)ptBuf - (uint32_t)TxData;
	if (uart_send(TxData, txfrSize))
		return COMM_FAIL;
	return COMM_SUCCESS;
}


