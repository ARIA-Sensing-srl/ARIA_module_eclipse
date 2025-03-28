/**
 * @file hal_evt_ctl.h
 * @brief event controller HAL
 *
 *  Created on: Dec 11, 2023\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */


#ifndef INC_HAL_EVT_CTL_H_
#define INC_HAL_EVT_CTL_H_



	#include "eventctrl.h"

#if defined(CORE_CV)
/**
 * Set the handler for the eventId, the routine don't enable the event generation
 * @param swEventNumber
 * @return SUCCESS/FAIL
 */
	int hal_evt_ctl_register(uint32_t eventId, void (*pfunc)(void *), void * param);

/**
 * Remove the event handler and disable the event generation
 * @param swEventNumber
 * @return SUCCESS/FAIL
 */
	int hal_evt_ctl_unregister(uint32_t eventId);

/**
 * Enable the handling of the event
 * @param swEventNumber
 * @return SUCCESS/FAIL
 */
	int hal_evt_ctl_unmask(uint32_t eventId);


/**
 * Disable the handling of the event
 * @param swEventNumber
 * @return SUCCESS/FAIL
 */
	int hal_evt_ctl_mask(uint32_t eventId);

#endif
/**
 * Trig a SW event accoring to the event number
 * @param swEventNumber
 * @return SUCCESS/FAIL
 */
	int hal_evt_ctl_sw_event(uint8_t swEventNumber);

	#define HAL_EVT_CTL_SUCCESS 0
	#define HAL_EVT_CTL_FAIL -1

#endif /* INC_HAL_EVT_CTL_H_ */
