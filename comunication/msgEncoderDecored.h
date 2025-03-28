/**
 * @file msgEncoderDecored.h
 * @brief Communication encoding functions
 *
 * This functions perform byte stuff/destuff operation, the encoding is part of the UART protocol
 *
 *  Created on: 07/ago/2019\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef MSGENCODERDECORED_H_
#define MSGENCODERDECORED_H_


#include "stdint.h"

#define MSG_FRAME_START 0xFF
#define MSG_FRAME_STOP 0x00
#define MSG_FRAME_STUFF 0x80
#define MSG_FRAME_START_ALT 0xFE
#define MSG_FRAME_STOP_ALT 0x01
#define MSG_FRAME_STUFF_ALT 0x81

#define MSG_SUCCESS 0
#define MSG_FAILURE -1
#define MSG_DECODING_ERROR -2 	//returned when message is not consistent
#define MSG_ENCODE_FAILURE -3

/**
 * Encode the buffer
 * @param pSrc source buffer
 * @param pDst destination buffer (minimum output size must be calculated with MSGComputeEndodedLength
 * @param srcsize input buffer size
 * @return encoded size
 */
int MSGEncode(uint8_t * pSrc, uint8_t * pDst, uint16_t srcsize);

/**
 * Compute the encoded buffer size
 * @param pSrc source buffer
 * @param srcsize buffer size
 * @return Encoded buffer size
 */
int MSGComputeEndodedLength(uint8_t * pSrc, uint16_t srcsize);
/**
 * Decode the source buffer, decoding could be executed in-place (pSrc==pDst)
 * @param pSrc input buffer
 * @param pDst output buffer
 * @param srcsize input buffer size
 * @return Decoded buffer size
 */
int MSGDecode(uint8_t * pSrc, uint8_t * pDst, uint16_t srcsize);	//no restriction for pSrc and pDst

#endif /* MSGENCODERDECORED_H_ */
