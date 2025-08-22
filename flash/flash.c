/*
 * flash.c
 *
 *  Created on: Nov 11, 2024
 *      Author: andream
 */


#include "hal_spi.h"
#include "flash.h"

#define LOCAL_BUFFER_SIZE (256+4)
//FLASH COMMANDS
#define CMD_RD 		0x03
#define CMD_WREN 	0x06
#define CMD_WRDIS 	0x04
#define CMD_PP		0x02
#define CMD_RDSR	0x05
#define CMD_WRSR	0x01
#define CMD_BER32K	0x52
#define CMD_BER64K	0xD8
#define CMD_SER		0x20//0xD7
#define CMD_RDJDID	0x9F
#define CMD_PPQ		0x32	//quad page program



#define BLOCK32_SIZE 	FL_BLOCK32_SIZE
#define BLOCK64_SIZE 	FL_BLOCK64_SIZE
#define SECTOR_SIZE		FL_SECTOR_SIZE
#define BLANK_BYTE 		0xFF

#define SR_WIP_POS 	0
#define SR_WEL_POS 	1
#define SR_BP0_POS 	2
#define SR_BP1_POS 	3
#define SR_BP2_POS 	4
#define SR_BP3_POS 	5
#define SR_QE_POS	6
#define SR_SWRD_POS	7

static hal_spi_t spihandler;
static uint8_t buffer[LOCAL_BUFFER_SIZE];
static struct _flashInfo{
	uint32_t flashSize_byte;
	uint8_t manID;
	bool detected;
}flashInfo;

static uint8_t priv_read_sr();
static int priv_write_enable();
static int priv_write_disable();
static int priv_detectflash();
static int priv_enableQE();


int FL_init(uint32_t clkfreq_Hz){

	hal_spi_config spicfg;
	spicfg.baudrate = clkfreq_Hz;
	spicfg.channel = FL_SPI_CHANNEL;
	spicfg.cpha = 0;
	spicfg.cpol = 0;
	int ret = hal_spi_init(&spihandler, &spicfg);
	if (ret != 0){
		return FL_ERROR;
	}
	return priv_detectflash();
}

static int priv_detectflash(){
	buffer[0]=CMD_RDJDID;
	hal_spi_fullduplex(&spihandler, buffer, buffer, 4, FL_SPI_CS_NUM);
	flashInfo.manID = buffer[1];
	flashInfo.detected = false;
	if (flashInfo.manID == 0)
		return FL_ERROR;
	uint8_t capEncode;
	capEncode = buffer[3];
	flashInfo.detected = true;
	flashInfo.flashSize_byte = 1<<17; //set to 1Mbit (128kByte)
	if (capEncode >= 16 && capEncode <= 31){
		flashInfo.flashSize_byte = 1UL << capEncode;
	}else if (capEncode >= 32 && capEncode <= 37){
		flashInfo.flashSize_byte = 1UL << (capEncode-6);
	}
	//priv_enableQE();
	return 0;
}

int FL_deinit(){
	hal_spi_deinit(&spihandler);
	return 0;
}
int FL_erasesector(uint32_t sectorAddress){
	priv_write_enable();
	sectorAddress &= ~(SECTOR_SIZE-1); //sector aligned
	buffer[0] = CMD_SER;
	buffer[1] = (sectorAddress>>16) & 0xFF;
	buffer[2] = (sectorAddress>>8) & 0xFF;
	buffer[3] = (sectorAddress) & 0xFF;
	hal_spi_fullduplex(&spihandler, buffer, buffer, 4, FL_SPI_CS_NUM);

	for(;;){
		uint8_t sr = priv_read_sr();
		if (!(sr & (1 << SR_WIP_POS)))
			break;
	}
	priv_write_disable();
	return 0;
}

int FL_eraseblock32(uint32_t blockAddress){
	priv_write_enable();
	blockAddress &= ~(SECTOR_SIZE-1); //sector aligned
	buffer[0] = CMD_BER32K;
	buffer[1] = (blockAddress>>16) & 0xFF;
	buffer[2] = (blockAddress>>8) & 0xFF;
	buffer[3] = (blockAddress) & 0xFF;
	hal_spi_fullduplex(&spihandler, buffer, buffer, 4, FL_SPI_CS_NUM);

	for(;;){
		uint8_t sr = priv_read_sr();
		if (!(sr & (1 << SR_WIP_POS)))
			break;
	}
	priv_write_disable();
	return 0;
}

int FL_read(uint32_t address, uint8_t* pdest, int size){
	while(size){
		buffer[0] = CMD_RD;
		buffer[1] = (address >> 16) & 0xFF;
		buffer[2] = (address >> 8) & 0xFF;
		buffer[3] = (address) & 0xFF;
		int curTxfr = size;
		if (curTxfr  > (LOCAL_BUFFER_SIZE-4)){
			curTxfr = LOCAL_BUFFER_SIZE-4;
		}
		hal_spi_fullduplex(&spihandler, buffer, buffer, curTxfr+4, FL_SPI_CS_NUM);
		for(int i = 0; i< curTxfr; i++){
			*pdest++ = buffer[i+4];
		}
		size -= curTxfr;
		address += curTxfr;
	}
	return 0;
}

int FL_write(uint32_t address, uint8_t* psrc, int size){
	priv_write_enable();
	while(size){
		buffer[0] = CMD_PP;
		buffer[1] = (address >> 16) & 0xFF;
		buffer[2] = (address >> 8) & 0xFF;
		buffer[3] = (address) & 0xFF;
		int curTxfr = size;
		if (curTxfr  > (LOCAL_BUFFER_SIZE-4)){
			curTxfr = LOCAL_BUFFER_SIZE-4;
		}
		for(int i = 0; i < curTxfr; i++){
			buffer[4+i] = *psrc++;
		}
		hal_spi_fullduplex(&spihandler, buffer, buffer, curTxfr+4, FL_SPI_CS_NUM);
		size -= curTxfr;
		address += curTxfr;
		for(;;){
			uint8_t sr = priv_read_sr();
			if (!(sr & (1 << SR_WIP_POS)))
				break;
		}
	}
	priv_write_disable();
	return 0;
}

int FL_masserase(){
	int numBlocks32 = flashInfo.flashSize_byte/BLOCK32_SIZE;
	int sectorsPerBlock32 =  BLOCK32_SIZE/SECTOR_SIZE;

	//initial block store bootloader, first block are reserved,
	//erase sectors for first block
	for (int i = (FL_BOOT_NUM_SECTORS); i < sectorsPerBlock32; i++){
		uint32_t sectorAddr = i*SECTOR_SIZE;
		FL_erasesector(sectorAddr);
	}
	//erase blocks from 1 to end
	for(int i = 1; i < numBlocks32; i++){
		uint32_t blockAddress = i*BLOCK32_SIZE;
		FL_eraseblock32(blockAddress);
	}
	return 0;
}

bool FL_isblank(uint32_t startaddress, uint32_t size){
	bool ret = true;
	while(size && ret){
		uint32_t txfr = size;
		if (txfr > LOCAL_BUFFER_SIZE-4)
			txfr = LOCAL_BUFFER_SIZE-4;
		FL_read(startaddress, buffer, txfr);
		startaddress += txfr;
		size -= txfr;
		for(int i = 0 ; i< txfr; i++){
			if (buffer[i]!= BLANK_BYTE){
				ret = false;
				break;
			}
		}
	}
	return ret;
}

uint32_t FL_getFlashSize(){
	if (flashInfo.detected)
		return flashInfo.flashSize_byte;
	return 0;
}

static int priv_write_enable(){
	buffer[0] = CMD_WREN;
	hal_spi_fullduplex(&spihandler, buffer, buffer, 1, FL_SPI_CS_NUM);
	return 0;
}
static int priv_write_disable(){
	buffer[0] = CMD_WRDIS;
	hal_spi_fullduplex(&spihandler, buffer, buffer, 1, FL_SPI_CS_NUM);
	return 0;
}

static uint8_t priv_read_sr(){
	buffer[0]=CMD_RDSR;
	hal_spi_fullduplex(&spihandler, buffer, buffer, 2, FL_SPI_CS_NUM);
	return buffer[1];
}


static int priv_enableQE(){
	uint8_t sr = priv_read_sr();
	if (!(sr & (1 << SR_QE_POS))){
		priv_write_enable();
		sr = (1 << SR_QE_POS);
		uint8_t buffer[2];
		buffer[0] = CMD_WRSR;
		buffer[1] = sr;
		hal_spi_fullduplex(&spihandler, buffer, buffer, 2, FL_SPI_CS_NUM);
		for(;;){
			sr = priv_read_sr();
			if (!(sr & (1 << SR_WIP_POS)))
				break;
		}
	}
	return 0;
}
