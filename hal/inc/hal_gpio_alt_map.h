/**
 * @file hal_gpio_alt_map.h
 * @brief PAD alternate mapping
 *
 *  Created on: May 31, 2024\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_INC_HAL_GPIO_ALT_MAP_H_
#define HAL_INC_HAL_GPIO_ALT_MAP_H_


//GPIO 0 altenrate mode
#define GPIO0_ALT_SPIMIO0		0
#define GPIO0_ALT_GPIO			1

#define GPIO1_ALT_SPIMIO1		0
#define GPIO1_ALT_GPIO			1

#define GPIO2_ALT_SPIMIO2		0
#define GPIO2_ALT_GPIO			1
#define GPIO2_ALT_SPIM1_CS1		2
#define GPIO2_ALT_TIM0_CH0		3

#define GPIO3_ALT_SPIMIO3		0
#define GPIO3_ALT_GPIO			1
#define GPIO3_ALT_SPIM0_CS2		2
#define GPIO3_ALT_TIM1_CH0		3

#define GPIO4_ALT_SPIM_CS0		0
#define GPIO4_ALT_GPIO			1

#define GPIO5_ALT_SPIM_CLK		0
#define GPIO5_ALT_GPIO			1

#define GPIO6_ALT_SPISIO0		0
#define GPIO6_ALT_SPIMIO0		1
#define GPIO6_ALT_GPIO			2
#define GPIO6_ALT_TIM2_CH0		3

#define GPIO7_ALT_SPISIO1		0
#define GPIO7_ALT_SPIMIO1		1
#define GPIO7_ALT_GPIO			2
#define GPIO7_ALT_TIM3_CH0		3

#define GPIO8_ALT_SPISIO2		0
#define GPIO8_ALT_SPIMIO2		1
#define GPIO8_ALT_GPIO			2
#define GPIO8_ALT_SPIM0_CS1		3

#define GPIO9_ALT_SPISIO3		0
#define GPIO9_ALT_SPIMIO3		1
#define GPIO9_ALT_GPIO			2
#define GPIO9_ALT_SPIM1_CS2		3

#define GPIO10_ALT_SPISCS		0
#define GPIO10_ALT_SPIM1_CS0	1
#define GPIO10_ALT_GPIO			2
#define GPIO10_ALT_TIM0_CH1		3

#define GPIO11_ALT_SPIS_CLK		0
#define GPIO11_ALT_SPIM1_CLK	1
#define GPIO11_ALT_GPIO			2
#define GPIO11_ALT_TIM1_CH1		3

#define GPIO12_ALT_URX			0
#define GPIO12_ALT_GPIO			1
#define GPIO12_ALT_SPIM0_CS3	2
#define GPIO12_ALT_TIM0_CH2		3

#define GPIO13_ALT_UTX			0
#define GPIO13_ALT_GPIO			1
#define GPIO13_ALT_SPIM1_CS3	2
#define GPIO13_ALT_TIM0_CH3		3



#endif /* HAL_INC_HAL_GPIO_ALT_MAP_H_ */
