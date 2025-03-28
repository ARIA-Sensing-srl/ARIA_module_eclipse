/**
 * @file hal_apbtmr.c
 * @brief Basic timer HAL
 *
 *  Created on: Jan 23, 2023\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */
#include "apbtimer.h"

#ifndef HAL_INC_HAL_APBTMR_H_
#define HAL_INC_HAL_APBTMR_H_

#define HAL_APBTMR_TMR_LO 0
#define HAL_APBTMR_TMR_HI 1

#define HAL_APBTMR_TMR_CASCODE 1
#define HAL_APBTMR_TMR_INDIPENDET 0

#define HAL_APBTMR_CLKSRC_SYSCLK 0
#define HAL_APBTMR_CLKSRC_LFO 1

#define HAL_APBTMR_MODE_CONT 0		//when reach compare timer restart from 0
#define HAL_APBTMR_MODE_ONESHOT 1


/** @struct _hal_apb_config_t
 *  basic timer configuration structure
 *  @var _hal_apb_config_t::tmrhilo
 *  Channel to configure (LO/HI)
 *  @var _hal_apb_config_t::cascode
 *  Enable cascode mode for 64 bit timer
 *  @var _hal_apb_config_t::prescaler
 *  Select the prescaler
 *  @var _hal_apb_config_t::precsaler_en
 *  Enable the prescaler
 *  @var _hal_apb_config_t::clksrc
 *  Select timer clock source
 *  @var _hal_apb_config_t::mode
 *  Select timer mode (continous/oneshot)
 *  @var _hal_apb_config_t::compare
 *  Select counter match
 */
typedef struct _hal_apb_config_t{
	uint8_t tmrhilo;
	uint8_t cascode;
	uint8_t prescaler;
	uint8_t precsaler_en;
	uint8_t clksrc;
	uint8_t mode;
	uint32_t compare;
}hal_apb_config_t;

typedef int _hal_apbtr_result_t;
/**
 * Basic timer configuration
 * @param ptimer timer handler
 * @param config configuration
 * @return SUCCESS/FAIL
 */
_hal_apbtr_result_t hal_apbtmr_config(volatile apb_timer_t* ptimer, hal_apb_config_t config);
/**
 * Reset the basic timer
 * @param ptimer timer handler
 * @param hilo channel
 * @return SUCCESS/FAIL
 */
_hal_apbtr_result_t hal_apbtmr_reset(volatile apb_timer_t* ptimer, uint8_t hilo);
/**
 * Start the basic timer
 * @param ptimer handler
 * @param hilo channel
 * @return SUCCESS/FAIL
 */
_hal_apbtr_result_t hal_apbtmr_start(volatile apb_timer_t* ptimer, uint8_t hilo);
/**
 * Enable interrupt generation
 * @param ptimer handler
 * @param hilo channel
 * @param enable
 * @return SUCCESS/FAIL
 */
_hal_apbtr_result_t hal_apbtmr_int(volatile apb_timer_t* ptimer, uint8_t hilo, uint8_t enable);
/**
 * Clear pending interrupt request
 * @param ptimer handler
 * @param hilo channel
 * @return SUCCESS/FAIL
 */
_hal_apbtr_result_t hal_apbtmr_int_pending_clear(volatile apb_timer_t* ptimer, uint8_t hilo);

#define HAL_APBTMR_SUCCESS 0
#define HAL_APBTMR_FAIL -1

#endif /* HAL_INC_HAL_APBTMR_H_ */
