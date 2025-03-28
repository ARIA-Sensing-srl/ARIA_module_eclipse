/**
 * @file exception.c
 * @brief exception and default interrupt routines
 *
 * The interrupt routines are defined as weak, user should re-implement the routine or edit the current file
 *  Created on: Feb 1, 2024\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#include "cv32e40_core.h"
#include "stdint.h"


/**
 * exception routine, called in case of HW exception
 */
__attribute__((interrupt ("machine")))
void exception_handler(){
	volatile uint32_t mcause = hal_spr_read(CSR_MCAUSE);
	volatile uint32_t mepc = hal_spr_read(CSR_MEPC);
	//check intterrup
	if (mcause & CSR_MCAUSE_INTERRUPT_MASK){
		//unexpected, normally vectorized interrupt
		return;
	}
	switch(mcause & CSR_MCAUSE_EXCEPTION_MASK){
		case CSR_EXCEP_BREAKPOINT:
		case CSR_EXCEP_ECALL:
			//exit, nothing to do
			return;
			break;
		case CSR_EXCEP_INSTR_ILLEGAL:
		default:
			while(1);
			break;
	}
}


__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_timer_lo(){};

__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_timer_hi(){};

__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_rise_fall(){};

__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_gpio(){};

__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_adv_tmr0(){};

__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_adv_tmr1(){};

__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_adv_tmr2(){};

__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_adv_tmr3(){};

__attribute__((interrupt ("machine")))__attribute__((weak))
void isr_evt_fifo_valid(){};







