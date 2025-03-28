/**
 * @file hal_intc.c
 * @brief interrupt contrller HAL
 *
 *  Created on: Jan 23, 2023\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_INC_HAL_INTC_H_
#define HAL_INC_HAL_INTC_H_

#include "interrupts.h"
#include "stdbool.h"

typedef int _hal_intc_result;
/**
 * Enable interrupt propagation
 * @param id
 * @return SUCCESS/FAIL
 */
_hal_intc_result hal_intc_enable(int id);
/**
 * Disable interrupt propagation
 * @param id
 * @return SUCCESS/FAIL
 */
_hal_intc_result hal_intc_disable(int id);
/**
 * Get the status of the selected interrupt
 * @param id
 * @return true if assert, false otherwise
 */
bool hal_intc_status(int id);
/**
 * Clear the interrupt status register of the selected interrupt
 * @param id
 * @return SUCCESS/FAIL
 */
_hal_intc_result hal_intc_clear(int id);

#define HAL_ITC_SUCCESS 0
#define HAL_ITC_FAIL -1



#endif /* HAL_INC_HAL_INTC_H_ */
