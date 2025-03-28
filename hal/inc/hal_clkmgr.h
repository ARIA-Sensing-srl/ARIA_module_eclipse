/**
 * @file hal_clkmgr.h
 * @brief Clock management HAL
 *
 *  Created on: Jan 14, 2025\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_APP_LIBONLY_INC_HAL_CLKMGR_H_
#define HAL_APP_LIBONLY_INC_HAL_CLKMGR_H_
#include "stdint.h"
/**
 * System clock source selection
 */
typedef enum _hal_clkmgh_cpuclk_src_t{
	CLKMGH_CPUCLK_SRC_RC,   /**< Internal 3 MHz frequency RC oscillator */
	CLKMGH_CPUCLK_SRC_XTAL, /**< Crystal */
	CLKMGH_CPUCLK_SRC_SYSCLK/**< PLL */
}hal_clkmgh_cpu_clksrc_t;
/**
 * PLL post divider CPU output
 */
typedef enum _hal_clkmgh_sysclk_div_t{
	CLKMGH_SYSCLK_DIV_8, /**< 1/8 */
	CLKMGH_SYSCLK_DIV_16,/**< 1/16 */
	CLKMGH_SYSCLK_DIV_32,/**< 1/32 */
	CLKMGH_SYSCLK_DIV_64 /**< 1/64 */
}hal_clkmgh_sysclk_div_t;
/**
 * Sysclk divider peripheral clock
 */
typedef enum _hal_clkmgh_perdiv_t{
	CLKMGH_PERCLKDIV_2 = 0,  /**< 1/2 */
	CLKMGH_PERCLKDIV_4 = 1,  /**< 1/4 */
	CLKMGH_PERCLKDIV_8 = 2,  /**< 1/8 */
	CLKMGH_PERCLKDIV_16 = 3, /**< 1/16 */
	CLKMGH_PERCLKDIV_32 = 4, /**< 1/32 */
	CLKMGH_PERCLKDIV_64 = 5, /**< 1/64 */
	CLKMGH_PERCLKDIV_128 = 6,/**< 1/128 */
	CLKMGH_PERCLKDIV_256 = 7,/**< 1/256 */
}hal_clkmgh_perdiv_t;

/**
 * Return current peripheral frequency in Hz
 * @return peripheral frequency in Hz
 */
uint32_t hal_clkmgh_get_peripheral_freq();
/**
 * Return current system frequency in Hz
 * @return sysclk frequency in Hz
 */
uint32_t hal_clkmgh_get_sys_freq();
/**
 * Return main PLL frequency in MHz
 * @return main PLL frequency in MHz
 */
uint32_t hal_clkmgh_get_pll_frequency_MHz();
/**
 * Set sysclk clock source
 * @param clksrc
 * @return SUCCESS/FAIL
 */
uint32_t hal_clkmgh_set_cpuclk_src(hal_clkmgh_cpu_clksrc_t clksrc);
/**
 * Get current sysclk clock
 * @return selected clock source
 */
hal_clkmgh_cpu_clksrc_t hal_clkmgh_get_cpuclk_src();
/**
 * Set current PLL post divider
 * @param div ratio
 * @return SUCCESS/FAIL
 */
uint32_t hal_clkmgh_set_sysclk_div(hal_clkmgh_sysclk_div_t div);
/**
 * Get current PLL post divider ratio
 * @return post divider ratio
 */
hal_clkmgh_sysclk_div_t hal_clkmgh_get_sysclk_div();
/**
 * Set sysclk divider ratio for peripherals
 * @param div ratio
 * @return SUCCESS/FAIL
 */
uint32_t hal_clkmgh_set_perclk_div(hal_clkmgh_perdiv_t div);
/**
 * Get peripheral sysclk divider ratio
 * @return sysclk divider ratio
 */
hal_clkmgh_perdiv_t hal_clkmgh_get_perclk_div();


#endif /* HAL_APP_LIBONLY_INC_HAL_CLKMGR_H_ */
