/**
 * @file main.h
 * Defines global variables, and extern reference for global functions
 *  Created on: Jan 26, 2024\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stdbool.h"

#define SYSTEM_TICK_ms 				1
#define SYSTEN_TICK_FREQ_HZ 		1000
#define SYSTEM_XTAL_CLOCK_FREQ_HZ 	32000000UL
#define SYSTEM_PLL_FREQ_MHZ 		1792UL


extern volatile uint32_t glb_sysTick; /**< System timer global variable */
extern volatile bool glb_errorstate;  /**< System error state */

/**
 * Delay function based on system timer
 * @param delay_ms required delay in ms
 */
extern void HAL_Delay(uint32_t delay_ms);
/**
 * Return the system timer ticks value
 * @return glb_sysTick value
 */
extern volatile uint32_t HAL_getSysTick();
/**
 * Return the elapsed time from a start timestamp
 * @param start initial time
 * @return elapsed time
 */
extern uint32_t HAL_getElapsed(uint32_t start);


#endif /* MAIN_H_ */
