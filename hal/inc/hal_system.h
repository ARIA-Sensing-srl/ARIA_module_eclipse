/**
 * @file hal_system.h
 * @brief System initialization HAL
 *
 *
 *  Created on: Jan 14, 2025\n
 *      Author: ARIA Sensing\n
 *      Copyright: ARIA Sensing, 2023 - \n
 */


#ifndef HAL_APP_LIBONLY_INC_HAL_SYSTEM_H_
#define HAL_APP_LIBONLY_INC_HAL_SYSTEM_H_
#include "stdint.h"
/**
 * System clock startup and initialization
 * @param xtalfreq_Hz external crystal frequency in Hz
 * @param refreq_Mhz PLL target frequency in MHz
 * @return SUCCESS/FAIL
 */
uint32_t hal_system_init(uint32_t xtalfreq_Hz, uint32_t refreq_Mhz);
/**
 * System clock update
 * @param refreq_Mhz
 * @return SUCCESS/FAIL
 */
uint32_t hal_system_updatepll(uint32_t refreq_Mhz);

#endif /* HAL_APP_LIBONLY_INC_HAL_SYSTEM_H_ */
