/*
 * msgEncoderDecored.c
 *
 *  Created on: 07/ago/2019
 *      Author: ARIA Sensing
 */


#include "msgEncoderDecored.h"
#include "stdbool.h"


int MSGEncode(uint8_t * psrc, uint8_t * pdst, uint16_t srcsize){
	if (psrc == pdst)
		return MSG_ENCODE_FAILURE;
	int retSize;
	retSize = srcsize;
	while(srcsize--){
		uint8_t cur;
		cur = *psrc++;
		switch(cur){
			case MSG_FRAME_START:
				*pdst++ = MSG_FRAME_STUFF;
				cur = MSG_FRAME_START_ALT;
				retSize++;
				break;
			case MSG_FRAME_STOP:
				*pdst++ = MSG_FRAME_STUFF;
				cur = MSG_FRAME_STOP_ALT;
				retSize++;
				break;
			case MSG_FRAME_STUFF:
				*pdst++ = MSG_FRAME_STUFF;
				cur = MSG_FRAME_STUFF_ALT;
				retSize++;
				break;
			default:
				break;
		}
		*pdst++ = cur;
	}
	return retSize;

}

int MSGComputeEndodedLength(uint8_t * pSrc, uint16_t srcsize){
	//count number of stuffed data
	int retVal = srcsize;
	while(srcsize--){
		switch(*pSrc++){
			case MSG_FRAME_START:
			case MSG_FRAME_STOP:
			case MSG_FRAME_STUFF:
				retVal++;
				break;
			default:
				break;
		}
	}
	return retVal;
}


int MSGDecode(uint8_t * pSrc, uint8_t * pDst, uint16_t srcsize){
	bool stuff = false;
	int outputSize;
	outputSize = srcsize;
	while(srcsize--){
		uint8_t curChar;
		curChar = *pSrc++;
		if (stuff){
			stuff=false;
			outputSize--; //decrement output size (removed stuff byte)
			switch (curChar){
				case MSG_FRAME_START_ALT:
					curChar = MSG_FRAME_START;
					break;
				case MSG_FRAME_STOP_ALT:
					curChar = MSG_FRAME_STOP;
					break;
				case MSG_FRAME_STUFF_ALT:
					curChar = MSG_FRAME_STUFF;
					break;
				default:
					return MSG_DECODING_ERROR; //malformed message
			}
		}else{
			stuff = (curChar == MSG_FRAME_STUFF);
			if (stuff){
				continue;
			}
		}
		*pDst++=curChar;
	}
	if (stuff)
		return MSG_DECODING_ERROR; //malformed
	return outputSize;
}
