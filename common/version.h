/**
 * @file version.h
 * @brief Version string definition according to FW model
 *  Created on: Nov 18, 2023
 *      Author: ARIA Sensing
 */

#ifndef VERSION_H_
#define VERSION_H_

#include <stdint.h>
#include "models.h"


#define CUSTOMER_CODE 	0x0000
#if DEF_BOARD_MODEL == AHM2D
#define HW_CODE			0xA2D0
#elif DEF_BOARD_MODEL == AHM3D
#define HW_CODE			0xA3D0
#elif DEF_BOARD_MODEL == AHM3DSC
#define HW_CODE			0xA3D5
#elif (DEF_BOARD_MODEL == AHM3DSC_RC2) || (DEF_BOARD_MODEL == AHM3DSC_RC1)
#define HW_CODE			0xA3D5
#elif (DEF_BOARD_MODEL == AHM2DSC)
#define HW_CODE			0xA2D5
#elif (DEF_BOARD_MODEL == AHM2DL)
#define HW_CODE			0xA2D1
#else
#error "Undefined board model"
#endif

#define FeatureID		0x0
#define FWIDVer			0x0
#define FWSubVer		0x1

#define VERSION_BYTE_STREAM {	\
	(CUSTOMER_CODE) & 0xFF,		\
	(CUSTOMER_CODE>>8) & 0xFF, 	\
	(HW_CODE) & 0xFF,			\
	(HW_CODE>>8) & 0xFF,		\
	(FWIDVer) & 0xFF,			\
	(FWSubVer) & 0xFF,			\
	(FWSubVer>>8) & 0xFF,		\
	(FWSubVer>>16) & 0xFF,		\
}



typedef struct Version
{
	uint8_t major;
	uint8_t minor;
}v;


#endif /* VERSION_H_ */
