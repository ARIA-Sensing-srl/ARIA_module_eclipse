/*
 * NVM.c
 *
 *  Created on: Aug 19, 2025
 *      Author: andream
 */

/*
 * Data structure 0-3 magic, 4-5 CRC16 (all sector) data
 * Key (0 tem str)
 * 1 type
 * 1 length (number of items)
 * */

#include "flash.h"
#include "NVM.h"
#include "string.h"
#include "stdint.h"

#define DEF_FLASH_FREQ 10000000UL
#define POLYNOM 0x1021
#define MAGIC 0x1122AABB
#define MAGIC_BYTE_POS 0
#define CRC_BYTE_POS 4
#define DATA_START_POS 6
#define MAX_ARRAY_LGTH 256


typedef struct _item_t{
	uint8_t * secPt;	//entry start
	uint8_t * pdata;	//data start
	NVMTypes_t type;
	int size;
}item_t;

static uint8_t sector[FL_SECTOR_SIZE] __attribute__((sector(".l2")));
static uint32_t flashNVMAddress;
static bool isinit = false;
static bool isopen = false;
static bool isdirty = false;


static int priv_loadSector(uint32_t  address);
static int priv_writeSector(uint32_t  address);
static int priv_verifyIntegrity();
static int priv_initNVM();
static uint16_t priv_crc16(uint8_t *p, int size);
static item_t priv_getKey(char * keyStr); //
static int priv_getRecordLength(item_t);



int NVM_init(){
	isinit = false;
	isopen = false;
	if (FL_init(DEF_FLASH_FREQ)){
		return NVM_ERROR;
	}
	flashNVMAddress = FL_getFlashSize()-FL_SECTOR_SIZE;
	if (flashNVMAddress == 0){
		return NVM_ERROR;
	}

	isinit = true;
	return NVM_SUCCESS;

}


int NVM_read(char * key, void * p){
	//look for key, zero length string mask the end, return number of bytes
	if (!isopen){
		return NVM_ERROR_CLOSE;
	}
	item_t t = priv_getKey(key);
	if (t.secPt == NULL){
		return NVM_ERROR_KEY_NOT_FOUND;
	}

	int size = t.size;
	if (t.type == NVM_16){
		size *= 2;
	}else if (t.type == NVM_32){
		size *= 4;
	}
	uint8_t * pdata;
	pdata = (uint8_t*)p;
	for(int i = 0 ; i< size; i++){
		pdata[i]=t.pdata[i];
	}
	return size;
}
int NVM_erase(char * key){
	if (!isopen)
		return NVM_ERROR_CLOSE;

	item_t t = priv_getKey(key);
	if (t.secPt == NULL){
		return NVM_ERROR_KEY_NOT_FOUND;
	}
	//overwrite the key with all data
	isdirty = true;
	uint8_t * nextKeyPos;
	uint8_t * p = t.secPt;

	nextKeyPos = t.secPt + priv_getRecordLength(t);
	//copy all data from this point to
	uint8_t * pend = &sector[sizeof(sector)];
	while(nextKeyPos != pend){
		*p++ = *nextKeyPos++;
	}
	return NVM_SUCCESS;
}
int NVM_write(char * key, NVMTypes_t type, int size ,void * p){
	//if current key exist, delete
	if (!isopen)
		return NVM_ERROR_CLOSE;
	if (size > 255){
		return NVM_ERROR_TOO_LARGE;
	}

	NVM_erase(key);

	//look for key pos
	uint8_t * ppos = &sector[DATA_START_POS];
	while(*ppos){
		item_t t = priv_getKey(ppos);
		if (t.secPt == NULL){
			return NVM_ERROR;
		}
		ppos += priv_getRecordLength(t);
	}
	//ppos as reached the wanted position
	int dataSize = size;
	if (type == NVM_16)
		dataSize *= 2;
	else if (type == NVM_32)
		dataSize *= 4;
	int totalSize = strlen(key)+3+dataSize;
	uint32_t resSize = (uint32_t)(&sector[sizeof(sector)]-ppos)-1; //1 space extra for 0
	if (resSize < totalSize){
		return NVM_ERROR_TOO_LARGE;
	}
	//copy data
	strcpy(ppos, key);
	ppos += strlen(key)+1;
	*ppos++ = type;
	*ppos++ = size;
	uint8_t * pdata = (uint8_t*)p;
	for (int i = 0; i < dataSize; i++){
		*ppos++ = *pdata++;
	};
	*ppos = 0; //mark last item
	isdirty = true;
	return NVM_SUCCESS;
}

int NVM_open(){
	if (!isinit)
		return NVM_ERROR;
	if (priv_loadSector(flashNVMAddress)){
		return NVM_ERROR;
	}
	if (priv_verifyIntegrity()){
		return NVM_ERROR_INTEGRITY;
	}
	isopen = true;
	isdirty = false;
	return NVM_SUCCESS;
}
int NVM_close(){
	isopen = false;
	if (isdirty){
		isdirty = false;
		if (priv_writeSector(flashNVMAddress)){
			return NVM_ERROR;
		}
	}
	return NVM_SUCCESS;
}

static item_t priv_getKey(char * keyStr){
	uint8_t * pcur = &sector[DATA_START_POS];
	item_t itemData;
	while(*pcur){
		//check key string
		int len = strlen(pcur)+1; //actual length is +1
		itemData.secPt = pcur;
		itemData.pdata = pcur+len+2;
		itemData.type = pcur[len];
		itemData.size = pcur[len+1];
		if (strcmp(pcur, keyStr)){
			//no match
			pcur = itemData.pdata;
			switch (itemData.type){
				case NVM_8:
					pcur += itemData.size;
					break;
				case NVM_16:
					pcur += itemData.size*2;
					break;
				case NVM_32:
					pcur += itemData.size*4;
					break;
				default:
					//abnormal
					itemData.pdata=NULL;
					itemData.secPt=NULL;
					return itemData;
					break;
			}
		}else{
			break; //match, exit
		}

	}
	if (*pcur == 0){
		itemData.pdata=NULL;
		itemData.secPt=NULL;
	}
	return itemData;
}

static int priv_getRecordLength(item_t t){
	int len = strlen(t.secPt)+1;
	len += 2; //type data
	switch(t.type){
		case NVM_8:
			len+= t.size;
			break;
		case NVM_16:
			len+= t.size*2;
			break;
		case NVM_32:
			len += t.size*4;
	}
	return len;
}

static int priv_loadSector(uint32_t  address){
	if (FL_read(address, sector, FL_SECTOR_SIZE))
		return -1;
	return 0;
}
static int priv_writeSector(uint32_t  address){
	//esare sector
	//update CRC prior
	*((uint16_t*)&(sector[CRC_BYTE_POS])) = 0; //reset current CRC
	uint16_t compCRC = priv_crc16(sector, sizeof(sector));


	*((uint16_t*)&(sector[CRC_BYTE_POS])) = compCRC;

	if (FL_erasesector(address))
		return -1;
	if (FL_write(address, sector, sizeof(sector)))
		return -1;
	return 0;
}

int NVM_format(){
	if (!isinit)
		return NVM_ERROR;

	if (priv_initNVM()){
		return NVM_ERROR;
	}
	return 0;
}

static uint16_t priv_crc16(uint8_t *p, int size){
	unsigned char i;
	uint16_t crcValue = 0x0000;
	for (int b = 0 ; b < size ; b++){
		uint8_t newByte = p[b];
		for (i = 0; i < 8; i++) {

			if (((crcValue & 0x8000) >> 8) ^ (newByte & 0x80)){
				crcValue = (crcValue << 1)  ^ POLYNOM;
			}else{
				crcValue = (crcValue << 1);
			}

			newByte <<= 1;
		}
	}
	return crcValue;
}

static int priv_verifyIntegrity(){
	uint32_t magic = *((uint32_t*)&(sector[MAGIC_BYTE_POS]));
	if (magic != MAGIC){
		return -1;
	}
	//check CRC
	uint16_t curCRC = *((uint16_t*)&(sector[CRC_BYTE_POS]));
	*((uint16_t*)&(sector[CRC_BYTE_POS])) = 0;


	uint16_t crc = priv_crc16(sector, sizeof(sector));

	*((uint16_t*)&(sector[CRC_BYTE_POS])) = curCRC;
	if (crc != curCRC)
		return -1;
	return 0; //integrity verified
}
static int priv_initNVM(){
	for(int i = 0; i<sizeof(sector); i++){
		sector[i] = 0xFF;
	}

	//set magic
	*((uint32_t*)&(sector[MAGIC_BYTE_POS])) = MAGIC;
	sector[DATA_START_POS] = 0;
	if (priv_writeSector(flashNVMAddress))
		return -1;

	//reload sector
	if (priv_loadSector(flashNVMAddress))
		return -1;
	//verify integrity
	if (priv_verifyIntegrity()){
		return -1;
	}
	return 0;
}

