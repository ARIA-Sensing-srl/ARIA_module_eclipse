/**
 * @file hal_advtmr.h
 * @brief Advanced timer HAL
 *
 *  Created on: Nov 6, 2024\n
 *      Author: ARIA Sensing\n
 *      Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_INC_HAL_ADVTMR_H_
#define HAL_INC_HAL_ADVTMR_H_

#include "advancedtimer.h"

#define HAL_ADVTMR0_ID 0
#define HAL_ADVTMR1_ID 1
#define HAL_ADVTMR2_ID 2
#define HAL_ADVTMR3_ID 3

#define HAL_ADVTMR_CH0_ID 0
#define HAL_ADVTMR_CH1_ID 1
#define HAL_ADVTMR_CH2_ID 2
#define HAL_ADVTMR_CH3_ID 3

#define HAL_ADVTMR_EVTSEL0 0
#define HAL_ADVTMR_EVTSEL1 1
#define HAL_ADVTMR_EVTSEL2 2
#define HAL_ADVTMR_EVTSEL3 3

/**
 * Advanced timer counter enable trigger mode
 */
typedef enum _hal_advtmr_trigmode_t{
	ADVTMR_TRIGMODE_CONT = 0,                    /**< continuous */
	ADVTMR_TRIGMODE_INPUT_LVL_0 = 1,             /**< Count when input level is 0 */
	ADVTMR_TRIGMODE_INPUT_LVL_1 = 2,             /**< Count when input level is 1 */
	ADVTMR_TRIGMODE_INPUT_EDGE_RISING = 3,       /**< Count on input rising edge */
	ADVTMR_TRIGMODE_INPUT_EDGE_FALLING = 4,      /**< Count on input falling edge */
	ADVTMR_TRIGMODE_INPUT_EDGE_BOTH = 5,         /**< Count on input both edges */
	ADVTMR_TRIGMODE_INPUT_EDGE_ARMED_RISING = 6, /**< Count on input rising edge and if armed */
	ADVTMR_TRIGMODE_INPUT_EDGE_ARMED_FALLING = 7,/**< Count on input falling edge and if armed */
}hal_advtmr_trigmode_t;
/**
 * Advanced timer counting mode selection
 */
typedef enum _hal_advtmr_countmode_t{
	ADVTMR_CNTMODE_ALTERNATE=0,		/**< up/down counting */
	ADVTMR_CNTMODE_UP=1				      /**< up count, reset on match */
}hal_advtmr_countmode_t;
/**
 * Advanced timer source clock selection
 */
typedef enum _hal_advtmr_clksrc_t{
	ADVTMR_CLKSRC_SYSTEM = 0,/**< System clock  */
	ADVTMR_CLKSRC_SLOWCLK = 1/**< Low frequency clock */
}hal_advtmr_clksrc_t;

/**
 * Advanced timer, channel behaviour mode
 */
typedef enum _hal_advtmr_ch_mode_t{
	ADVTMR_CHMODE_SET = 0,				/**< set output on threshold match */
	ADVTMR_CHMODE_TOGGLE_RST = 1,		/**< reset output on threshold match */
	ADVTMR_CHMODE_PWM_POL0 = 2,  		/**< PWM mode output */
	ADVTMR_CHMODE_TOGGLE = 3,			/**< toggle every match event */
	ADVTMR_CHMODE_CLR = 4,				/**< clear output on match */
	ADVTMR_CHMODE_TOGGLE_SET = 5,		/**< Polarity toggle */
	ADVTMR_CHMODE_PWM_POL1 = 6,  		/**< PWM mode output */
}hal_advtmr_ch_mode_t;

/** @struct _hal_advtmr_ch_cfg_t
 * Channel configuration structure
 * @var _hal_advtmr_ch_cfg_t::threshold
 * Internal compare threshold
 * @var _hal_advtmr_ch_cfg_t::mode
 * Output compare mode behavior
 *
 */
typedef struct _hal_advtmr_ch_cfg_t{
	uint16_t threshold;
	hal_advtmr_ch_mode_t mode;
}hal_advtmr_ch_cfg_t;

/** @struct _hal_advtmr_cfg_t
 * Advanced timer configuration structure
 * @var _hal_advtmr_cfg_t::lowcount
 * Set the lower threshold when dual ramp is selected
 * @var _hal_advtmr_cfg_t::upcount
 * Set the up compare threshold
 * @var _hal_advtmr_cfg_t::clksrc
 * Select the clock source
 * @var _hal_advtmr_cfg_t::mode
 * Select the count mode (up-down, up and reset)
 * @var _hal_advtmr_cfg_t::trigmode
 * Select the counter enable mode
 * @var _hal_advtmr_cfg_t::prescaler
 * Set the counter prescaler
 *
 */
typedef struct _hal_advtmr_cfg_t{
	uint16_t lowcount;
	uint16_t upcount;
	hal_advtmr_clksrc_t clksrc;
	hal_advtmr_countmode_t mode;
	hal_advtmr_trigmode_t trigmode;
	uint16_t prescaler;
}hal_advtmr_cfg_t;


//encode input source for configuration
#define mHAL_ADVTMR_INPUTSRC_GPIO(x) (x)
#define mHAL_ADVTMR_INPUTSRC_TMR(tmrID, channelid) (4*tmrID+channelid+32)

//macro for encode event request
#define mHAL_ADVTMR_EVTSRC(tmrID, channelID) (4*tmrID+ channelID)
/**
 * Return advanced timer peripheral handler
 * @param TmrID HAL_ADVTMRx_ID
 * @return advanced timer handler
 */
adv_tmr_t * hal_advtmr_getPeripheralHandler(uint8_t TmrID);
/**
 * Configure the advanced timer
 * @param phandler
 * @param config
 * @return SUCCESS/FAIL
 */
int hal_advtmr_configure(adv_tmr_t* phandler, hal_advtmr_cfg_t config);
/**
 * Configure a specific channel of the timer
 * @param phandler timer handler
 * @param channelID HAL_ADVTMR_CHx_ID
 * @param chconfig configuration structure
 * @return SUCCESS/FAIL
 */
int hal_advtmr_ch_configure(adv_tmr_t*phandler, int channelID, hal_advtmr_ch_cfg_t chconfig);
/**
 * Set channel threshold, used when PWM mode is configured
 * @param phandler
 * @param channelID
 * @param threshold
 * @return SUCCESS/FAIL
 */
int hal_advtmr_ch_setthreshold(adv_tmr_t*phandler, int channelID, uint16_t threshold);
/**
 * arm the advanced timer
 * @param phandler
 * @return SUCCESS/FAIL
 */
int hal_advtmr_arm(adv_tmr_t*phandler);
/**
 * Reset the advanced timer
 * @param phandler
 * @return SUCCESS/FAIL
 */
int hal_advtmr_rst(adv_tmr_t*phandler);
/**
 * Start the timer counter
 * @param phandler
 * @return SUCCESS/FAIL
 */
int hal_advtmr_start(adv_tmr_t*phandler);
/**
 * Stop the timer counter
 * @param phandler
 * @return SUCCESS/FAIL
 */
int hal_advtmr_stop(adv_tmr_t*phandler);
/**
 * Update configuration of the advanced timer
 * @param phandler
 * @return
 */
int hal_advtmr_update(adv_tmr_t*phandler);
/**
 * Configure event generation of the advanced timer
 * @param advtmr_evt
 * @param advtmr_evtsource
 * @return SUCCESS/FAIL
 */
int hal_advtmr_evt_cfg(int advtmr_evt, int advtmr_evtsource);
/**
 * Enable event generation
 * @param advtmr_evtsel
 * @return
 */
int hal_advtmr_evt_enable(int advtmr_evtsel);
/**
 * Disable event generation
 * @param advtmr_evtsel
 * @return
 */
int hal_advtmr_evt_disable(int advtmr_evtsel);
/**
 * Enable the clock of the selected timer
 * @param advtmrID
 * @return SUCCESS/FAIL
 */
int hal_advtmr_cg_enable(int advtmrID);
/**
 * Disable the clock of the selected timer
 * @param advtmrID
 * @return SUCCESS/FAIL
 */
int hal_advtmr_cg_disable(int advtmrID);

#define HAL_ADVTMR_SUCCESS 0
#define HAL_ADVTMR_FAIL -1


#endif /* HAL_INC_HAL_ADVTMR_H_ */
