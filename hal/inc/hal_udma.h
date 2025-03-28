/**
 * @file hal_udma.h
 * @brief UDMA core HAL
 *
 *  Created on: Jan 26, 2024\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_INC_HAL_UDMA_H_
#define HAL_INC_HAL_UDMA_H_


#include "hal_udma.h"
#include "udma.h"




typedef int _hal_udma_result_t;

/**
 * Enable uDMA peripheral clock
 * @param perid
 * @return SUCCESS/FAIL
 */
_hal_udma_result_t hal_udma_peripheral_ck_enable(int perid);
/**
 * Disable uDMA peripheral clock
 * @param perid
 * @return SUCCESS/FAIL
 */
_hal_udma_result_t hal_udma_peripheral_ck_disable(int perid);
/**
 * Reset all the uDMA peripherals
 * @return SUCCESS/FAIL
 */
_hal_udma_result_t hal_udma_peripheral_disable_rst_all();
/**
 * Reset uDMA peripheral
 * @param perid
 * @return SUCCESS/FAIL
 */
_hal_udma_result_t hal_udma_peripheral_reset(int perid);

#define HAL_UDMA_SUCCESS 0
#define HAL_UDMA_FAIL -1




#endif /* HAL_INC_HAL_UDMA_H_ */
