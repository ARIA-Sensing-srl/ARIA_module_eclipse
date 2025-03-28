/*
 * SoC_CTRL.h
*
 *  Created on: Oct 10, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef SOCCTRL_H_
#define SOCCTRL_H_

#include "soc_reg.h"

#include <stdint.h>
#include <stdbool.h>


	#define SOCCTRL_INFO_OFFSET 	(0x00000000)
	#define SOCCTRL_CVBOOT_OFFSET	(0x00000004)
	#define SOCCTRL_PADs_OFFSET		(0x00000010)

	#define SOCCTRL_COPR_OFFSET		(0x00000100)

	typedef struct _socctrl_pads_t{
		uint32_t PADMUX[4];
		uint32_t PADCFG[16];
	}socctrl_pads_t;

	typedef struct _socctrl_copr_t{
		uint32_t CR;
		uint32_t BOOTADDR;
	}socctrl_copr_t;

	#define SOCCTRL_INFO	(*(uint32_t*)(SOCCTRL_INFO_OFFSET+APB_SOC_CTRL_BASE))
	#define SOCCTRL_CVBOOT	(*(uint32_t*)(SOCCTRL_CVBOOT_OFFSET+APB_SOC_CTRL_BASE))
	#define SOCCTRL_PADCTRL	((volatile socctrl_pads_t*)(SOCCTRL_PADs_OFFSET+APB_SOC_CTRL_BASE))

	#define SOCTRL_COPR		((volatile socctrl_copr_t*)(SOCCTRL_COPR_OFFSET+APB_SOC_CTRL_BASE))


	//bits
	#define SOCCTRL_COPR_CR_NRST_BIT 0

#endif /* SOCCTRL_H_ */
