/*
 * hal_udma.c
 *
 *  Created on: Jan 26, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */
#include "hal_udma.h"
#include "udma.h"



#define CG_WAIT_CYCLEs 10
#define RST_WAIT_CYCLEs 10

	static void priv_wait(volatile uint32_t cyc);


	_hal_udma_result_t hal_udma_peripheral_ck_enable(int perid){
		if (perid > UDMA_MAX_PERID)
			return HAL_UDMA_FAIL;
		UDMA->CTRL_CFG_CG |= (1 << perid);
		priv_wait(CG_WAIT_CYCLEs);
		UDMA->CTRL_CFG_RST &= ~(1 << perid); //when enable release reset if already on
		priv_wait(RST_WAIT_CYCLEs);
		return HAL_UDMA_SUCCESS;
	}
	_hal_udma_result_t hal_udma_peripheral_ck_disable(int perid){
		if (perid > UDMA_MAX_PERID)
			return HAL_UDMA_FAIL;
		UDMA->CTRL_CFG_CG &= ~(1 << perid);
		priv_wait(CG_WAIT_CYCLEs);
		return HAL_UDMA_SUCCESS;
	}
	_hal_udma_result_t hal_udma_peripheral_disable_rst_all(){
		UDMA->CTRL_CFG_CG = 0;	//disable all
		priv_wait(CG_WAIT_CYCLEs);
		UDMA->CTRL_CFG_RST = ~0; //put all in reset state
		priv_wait(RST_WAIT_CYCLEs);
		UDMA->CTRL_CFG_RST = 0; //release reset
		return HAL_UDMA_SUCCESS;
	}
	_hal_udma_result_t hal_udma_peripheral_reset(int perid){
		UDMA->CTRL_CFG_RST |= (1 << perid);
		priv_wait(RST_WAIT_CYCLEs);
		UDMA->CTRL_CFG_RST &= ~(1 << perid);
		priv_wait(RST_WAIT_CYCLEs);
		return HAL_UDMA_SUCCESS;
	}


	static void priv_wait(volatile uint32_t cyc){
		while(cyc--){
			asm volatile("nop");
		}
	}


