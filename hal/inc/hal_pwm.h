/**
 * @file hal_pwm.h
 * @brief pulse-width modulation (PWM) HAL
 *
 * This HAL is designed on top of the advanced timer HAL to provide access to the PWM features of the timer\n
 * \n
 *  Created on: Nov 6, 2024\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_INC_HAL_PWM_H_
#define HAL_INC_HAL_PWM_H_

#include "hal_advtmr.h"


/**
 * PWM polarity mode selection
 */
typedef enum _hal_pwm_mode_t{
	PWM_MODE_POL0,	/**< output is asserted on match end reset on counter reset */
	PWM_MODE_POL1	 /**< output is de-asserted on match end reset on counter reset */
}hal_pwm_mode_t;

/**
 * @struct _hal_pwm_cfg_t
 * PWM configuration structure
 * @var _hal_pwm_cfg_t::advTmrID
 * Advanced timer ID to configure
 * @var _hal_pwm_cfg_t::advTmrChannel
 * Channel of the advanced timer
 * @var _hal_pwm_cfg_t::clksrc
 * Clock source
 * @var _hal_pwm_cfg_t::pwmmode
 * PWM output behavior mode
 * @var _hal_pwm_cfg_t::period
 * PWM period
 * @var _hal_pwm_cfg_t::prescaler
 * Timer prescaler
 */
typedef struct _hal_pwm_cfg_t{
	int advTmrID;
	int advTmrChannel;
	hal_advtmr_clksrc_t clksrc;
	hal_pwm_mode_t pwmmode;
	uint16_t period;
	uint16_t prescaler;
}hal_pwm_cfg_t;


typedef int hal_pwm_handler;

/**
 * Configure the PWM channel according to the configuration structure
 * @param cfg configuration structure
 * @return hal_pwm_handler (or HAL_PWM_HANDLER_INVALID if fail)
 */
hal_pwm_handler hal_pwm_config(hal_pwm_cfg_t cfg);
/**
 * Start the PWM channel
 * @param h handler
 * @return SUCCESS/FAIL
 */
int hal_pwm_start(hal_pwm_handler h);
/**
 * Stop the PWM channel
 * @param h
 * @return SUCCESS/FAIL
 */
int hal_pwm_stop(hal_pwm_handler h);
/**
 * Set the PWM threshold value
 * @param h
 * @param value
 * @return SUCCESS/FAIL
 */
int hal_pwm_set(hal_pwm_handler h, uint16_t value);


#define HAL_PWM_SUCCESS 0
#define HAL_PWM_FAIL -1

#define HAL_PWM_HANDLER_INVALID -1

#endif /* HAL_INC_HAL_PWM_H_ */
