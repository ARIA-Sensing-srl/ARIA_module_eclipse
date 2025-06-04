/**
 * @file
 * models.h
 * @brief Board models definition, used for set the correct board related parameter
 *
 *  Created on: Dec 4, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 *
 *
 *
 */



#ifndef INC_MODELS_H_
#define INC_MODELS_H_

#define AHM2D		0
#define AHM3D		1
#define AHM2DSC		2
#define AHM3DSC_RC1	3
#define AHM3DSC_RC2	4
#define AHM2DL		5

#ifndef PLATIO
#ifdef DEF_BOARD_MODEL
#error "DEF_BOARD_MODEL already defined"
#endif
/** @brief Board model selection */
#define DEF_BOARD_MODEL				AHM2DL
#else
#ifndef DEF_BOARD_MODEL
#error "Undefined board model"
#endif

#endif

#endif /* INC_MODELS_H_ */

/**
 * @page pgBrdModel Board model selection
 * \section Model selection
 * Select the required board models by editing the file models.h\n
 * @see DEF_BOARD_MODEL
 * */
