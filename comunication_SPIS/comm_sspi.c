/*
 * comm_sspi.c
 *
 *  Created on: May 31, 2024
 *      Author: ARIA Sensing
 *
 *      Host2Device:
 *      1. Host fille memory and assert req channels
 *      2. Device decode message and assert GNT signal
 *      3. Host deassert gnt
 *      4. Device deassert req
 *
 *      Device2Host
 *      1. Device fill memory and assert GNT
 *      2. Host read the memory and assert req
 *      3. Device deassert GNT
 *      4. Host Deassert REQ
 *
 *      Message is 2 byte for size and payload according to "size" field
 *      The bit15 of the payload is used for indicate to host that the transaction is splitted to multiple transactions (last transaction keep this to zero)
 *
 *		the slave SPI interface transfer data in 4 bytes words
 */


#include "comm_sspi.h"
#include "hal_gpio.h"
#include "hal_gpio_alt_map.h"
#include "radarcommands.h"
#include "stddef.h"
#include "string.h"


#define OPT_DISCRETE_HANDSHAKE 0
#define GPIO_REQ_PIN GPIO_PIN3
#define GPIO_GNT_PIN GPIO_PIN2
#define COMM_SSPI_SHARED_BUFFER_SIZE 4096
#define SEM_H2D_MASK (0x01000001)	//endiannes indipendent
#define SEM_D2H_MASK (0x00010100)	//endiannes indipendent



static enum _sm_states {SM_IDLE = 0, \
						SM_WAIT_REQ_DEASSERT, \
						SM_WAIT_REQ_ASSERT \
					}curstate;

static radar_command cmd;
static radar_command_payload payload;
static bool valid = false;

static volatile uint32_t semaphore __attribute__((section(".ioshared"))); //used for external handshake
static volatile uint8_t sharedBuffer[COMM_SSPI_SHARED_BUFFER_SIZE] __attribute__((section(".ioshared"))); //this is put on top of the shared address space
/*
 * Message structure is bytes 0-1 length, 2-n payload in the same format like the UART
 * The bit15 of the length idicate if the transaction is complete or something is still ongoing
 * */

static int priv_readincomingmessage();
static void priv_revendiannes(bool tohost); //interface reverse the endiannes, restore or scrumble

int comm_sspi_init(){
	hal_gpio_config_t cfg;
	cfg.mode = GPIO_MODE_ALT;

	//setup SPI slave mode
	cfg.pinnum = GPIO_PIN6;
	cfg.alt = GPIO6_ALT_SPISIO0;
	hal_gpio_configure(GPIO, &cfg);

	cfg.pinnum = GPIO_PIN7;
	cfg.alt = GPIO7_ALT_SPISIO1;
	hal_gpio_configure(GPIO, &cfg);

	cfg.pinnum = GPIO_PIN8;
	cfg.alt = GPIO8_ALT_SPISIO2;
	hal_gpio_configure(GPIO, &cfg);

	cfg.pinnum = GPIO_PIN9;
	cfg.alt = GPIO9_ALT_SPISIO3;
	hal_gpio_configure(GPIO, &cfg);

	cfg.pinnum = GPIO_PIN10;
	cfg.alt = GPIO10_ALT_SPISCS;
	hal_gpio_configure(GPIO, &cfg);

	cfg.pinnum = GPIO_PIN11;
	cfg.alt = GPIO11_ALT_SPIS_CLK;
	hal_gpio_configure(GPIO, &cfg);

	semaphore = 0; //this variable stay into an uninitialized section, bust be manually set

#if OPT_DISCRETE_HANDSHAKE==1
	//disable GPIO4
	cfg.defLvl = GPIO_LVL_SET;
	cfg.mode = GPIO_MODE_OUTPUT;
	cfg.pinnum = GPIO_PIN4;
	hal_gpio_configure(GPIO, &cfg);

	//configure handshake
	cfg.defLvl = GPIO_LVL_RESET;
	cfg.mode = GPIO_MODE_OUTPUT;
	cfg.pinnum = GPIO_GNT_PIN;
	hal_gpio_configure(GPIO, &cfg);

	cfg.mode = GPIO_MODE_INPUT;
	cfg.pinnum = GPIO_REQ_PIN;
	hal_gpio_configure(GPIO, &cfg);
#endif

	return 0;
}

int comm_sspi_task(){
#if OPT_DISCRETE_HANDSHAKE == 1
	enum _sm_states nextstate;
	nextstate = curstate;
	switch (curstate){
		case SM_IDLE:
			if (hal_gpio_get_level(GPIO, GPIO_REQ_PIN) == GPIO_LVL_SET){
				priv_readincomingmessage();
				hal_gpio_set_level(GPIO, GPIO_GNT_PIN, GPIO_LVL_SET);
				nextstate = SM_WAIT_REQ_DEASSERT;
			}
			break;
		case SM_WAIT_REQ_DEASSERT:
			if (hal_gpio_get_level(GPIO, GPIO_REQ_PIN) == GPIO_LVL_RESET){
				//handshake completed, deassert grant and go back to idle
				hal_gpio_set_level(GPIO, GPIO_GNT_PIN, GPIO_LVL_RESET);
				nextstate = SM_IDLE;
			}
			break;
		case SM_WAIT_REQ_ASSERT:
			//this state reached after a send request is issued, on send system rise the GNT and wait for REQ
			if (hal_gpio_get_level(GPIO, GPIO_REQ_PIN) == GPIO_LVL_SET){
				hal_gpio_set_level(GPIO, GPIO_GNT_PIN, GPIO_LVL_RESET);
				nextstate = SM_WAIT_REQ_DEASSERT; //wait deassert before restart with transaction
			}
			break;
		default:
			nextstate = SM_IDLE;
			break;
	}
	curstate = nextstate;
#else
	if (semaphore & SEM_H2D_MASK){
		//data into buffers
		priv_readincomingmessage();
		//clean
		semaphore &= ~SEM_H2D_MASK; //cleanup mask
	}
#endif
	return 0;

}

static int priv_readincomingmessage(){
	//check incomping buffer and decode message
	priv_revendiannes(false);

	uint16_t size = *((uint16_t*)sharedBuffer);
	size &= 0x7FFF;
	uint8_t * pdata = sharedBuffer+2; //data start from offset 2

	if (size == 0)
		return -1;

	uint8_t command_byte = *pdata++;
	const struct _RadarCommandItem * curCmdItem = NULL;

	for(int i=0; i < RDR_CMD_LIST_SIZE; i++){
		if (RadarCommandItem[i].cmdCode == command_byte){
			cmd = RadarCommandItem[i].cmd;
			curCmdItem = &RadarCommandItem[i];
			break;
		}
	}

	if (curCmdItem == NULL)
		return -1;

	switch (curCmdItem->payloadType){
		case PT_NO:
			break;
		case PT_U8_SR:
		case PT_U8:
			payload.data_uint8 =  *pdata++;
			break;
		case PT_U16_SR:
		case PT_U16:
			memcpy((void*)&payload.data_uint8, (const void * ) pdata, 2);
			pdata += 2;
			break;
		case PT_U32_SR:
		case PT_U32:
			memcpy((void*)&payload.data_uint8, (const void * ) pdata, 4);
			pdata += 4;
			break;
		case PT_FLOAT_SR:
		case PT_FLOAT:
			memcpy((void*)&payload.data_uint8, (const void * ) pdata, 4);
			pdata += 4;
			break;
		case PT_STRUCT_SR:
		case PT_STRUCT:
			size--;
			payload.data_struct.size = size;
			if (size > sizeof(payload.data_struct.buffer))
				return -1;
			if (size){
				memcpy((void*)&payload.data_struct.buffer, (const void * ) pdata, size);
			}
			break;
		default:
			return -1;
			break;
	}
	valid = true;
	return 0;
}

int comm_sspi_getmessage(radar_command* pcmd, radar_command_payload* ppayload){
	if (valid){
		*pcmd = cmd;
		*ppayload = payload;
		valid = false;
		return COMM_SSPI_SUCCESS;
	}
	return COMM_SSPI_NO_MESSAGE;

}
int comm_sspi_send_stream(uint8_t * pbuf, int size, bool isstart, bool isend, bool encode){
	//encode in this interface is ignored
#if OPT_DISCRETE_HANDSHAKE == 1
	while(curstate != SM_IDLE){
		comm_sspi_task();
	}
#else
	while(semaphore & SEM_D2H_MASK){
		asm volatile ("nop"); //wait until buffer is release
	}
#endif
	uint8_t * pdata = sharedBuffer;
	//put size
	*((uint16_t*)pdata) = isend ? size : (size | 0x8000); //if end is clear means no other messages chunks must be transmitted
	pdata += 2;
	//transfer buffer
	memcpy((void*)pdata, (const void*)pbuf, size);
	priv_revendiannes(true);
	//put state machine in wait REQ assert and set GNT
#if OPT_DISCRETE_HANDSHAKE == 1
	hal_gpio_set_level(GPIO, GPIO_GNT_PIN, GPIO_LVL_SET);
	curstate = SM_WAIT_REQ_ASSERT;
#else
	semaphore |= SEM_D2H_MASK; //assert remaphore data
#endif

	return 0;
}
int comm_sspi_send(comm_msgbuffer_t msgbuf){
	if (msgbuf.size > (COMM_SSPI_SHARED_BUFFER_SIZE)){
		return COMM_SSPI_NO_MEMORY;
	}
	return comm_sspi_send_stream(msgbuf.buffer, msgbuf.size, true, true, true);
}
int comm_sspi_getbuffer(int size, comm_msgbuffer_t* pbuf){
	if (size > (COMM_SSPI_SHARED_BUFFER_SIZE/2)){
		return COMM_SSPI_NO_MEMORY;
	}
	pbuf->buffer = &sharedBuffer[COMM_SSPI_SHARED_BUFFER_SIZE/2];
	pbuf->size = size;
	return 0;
}

static void priv_revendiannes(bool tohost){
	//tohost-> if assert means that the buffer must be sent to host (not inverted order)

	int size;
	uint32_t * pdata;
	uint32_t numWords;
	if (tohost){
		size = *((uint16_t*)sharedBuffer) + 2;
	} else {
		//from host
		size =((uint16_t)sharedBuffer[3] | ((uint16_t)(sharedBuffer[2]) << 8)) +2;
	}
	size &= 0x7FFF;
	pdata = (uint32_t*)sharedBuffer;
	numWords = (size)>>2; //also size section must be switched
	if ((size) & 0x3)
		numWords++;

	//execute
	uint32_t tdata;
	uint32_t shuffle;
	shuffle = (0<<24) | (1 << 16) | (2 << 8) | (3 << 0); //see pv.shuffle for encoding
	asm volatile(
			"lp.starti x1, lpa_s;"
			"lp.endi x1, lpa_e;"
			"lp.count x1, %[N];"
			".balign 4;"
			".option norvc;"
			"lpa_s:;"
			"lw %[t], (%[a]);"
			"pv.shuffle.b %[t], %[t], %[s];"
			"lpa_e:;"
			"p.sw %[t], 4(%[a]!);"
			: [t] "+r" (tdata), [a] "+rm" (pdata) : [N] "r" (numWords) , [s] "r" (shuffle)
			);
}

