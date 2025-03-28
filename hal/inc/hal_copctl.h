/**
 * @file hal_copctl.h
 * @brief Coprocessor control HAL
 *
 *  Created on: Jan 23, 2024\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_INC_HAL_COPCTL_H_
#define HAL_INC_HAL_COPCTL_H_

#include "stdint.h"


	typedef int _hal_copctl_result;
/**
 * Set the boot address of the co-processor
 * @param address
 * @return SUCCESS/FAIL
 */
	_hal_copctl_result hal_copctl_set_bootaddress(uint32_t address);
/**
 * Assert reset of the co-processor
 * @return SUCCESS/FAIL
 */
	_hal_copctl_result hal_copctl_rst_assert();
/**
 * De-assert the reset of the co-processor
 * @return SUCCESS/FAIL
 */
	_hal_copctl_result hal_copctl_rst_deassert();

#define HAL_COPCTL_SUCCESS 0
#define HAL_COPCTL_FAIL -1


#endif /* HAL_INC_HAL_COPCTL_H_ */
