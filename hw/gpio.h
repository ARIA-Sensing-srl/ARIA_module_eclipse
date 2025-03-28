/*
 * GPIO.h
*
 *  Created on: Oct 10, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "soc_reg.h"

#include <stdint.h>

	typedef struct _gpio_t{
		uint32_t PADDIR;
		uint32_t GPIOEN;
		uint32_t PADIN;
		uint32_t PADOUT;
		uint32_t PADOUTSET;
		uint32_t PADOUTCLR;
		uint32_t INTEN;
		uint32_t INTTYPE[2];
		uint32_t INTSTATUS;
		uint32_t PADCFG[4];
	}gpio_t;

	#define GPIO	((volatile gpio_t*)APB_GPIO_BASE)


	#define GPIO_INTTYPE_EDGEFALL_MASK 0x00
	#define GPIO_INTTYPE_EDGERISE_MASK 0x01
	#define GPIO_INTTYPE_EDGEBOTH_MASK 0x02

	#define GPIO_PIN0 		0
	#define GPIO_PIN1 		1
	#define GPIO_PIN2 		2
	#define GPIO_PIN3 		3
	#define GPIO_PIN4 		4
	#define GPIO_PIN5 		5
	#define GPIO_PIN6 		6
	#define GPIO_PIN7 		7
	#define GPIO_PIN8 		8
	#define GPIO_PIN9 		9
	#define GPIO_PIN10 		10
	#define GPIO_PIN11 		11
	#define GPIO_PIN12 		12
	#define GPIO_PIN13 		13


#endif /* GPIO_H_ */
