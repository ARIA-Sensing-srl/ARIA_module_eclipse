/*
 * main.c
*
 *  Created on: Jan 24, 2024
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */


#include <stdio.h>
#include <uart_app.h>
#include "ipc.h"
#include "apbtimer.h"
#include "interrupts.h"
#include "cv32e40_core.h"
#include "socctrl.h"
#include "hal_udma.h"
#include "hal_apbtmr.h"
#include "hal_copctl.h"
#include "hal_intc.h"
#include "hydrogen_UD_v1.h"
#include "main.h"
#include "hal_clkmgr.h"
#include "hal_system.h"
#include "models.h"

#ifndef SYSTEM_XTAL_CLOCK_FREQ_HZ
#error "Specify frequency of the crystal clock"
#endif
//** DEFINES START **//
#if (DEF_BOARD_MODEL == AHM3DSC_RC1) || (DEF_BOARD_MODEL == AHM3DSC_RC2)
#define DEF_FB_RX 4
#define DEF_FB_TX 1
#elif (DEF_BOARD_MODEL == AHM3D)
#define DEF_FB_RX 2
#define DEF_FB_TX 2
#else
#define DEF_FB_RX 4
#define DEF_FB_TX 4
#endif
//** DEFINES END **//

//** MACRO START **//
//** MACRO END **//

//** TYPES START **//
//** TYPES END **//

//** PRIVATE VARIABLES START **//
static HydrUDriver_t HydrHandler;
//** PRIVATE VARIABLES END **//

//** PUBLIC VARIABLES START **//
volatile uint32_t glb_sysTick;
volatile bool glb_errorstate;
extern uint32_t __copr_start;
//** PUBLIC VARIABLES END **//

//** PRIVATE FUNCTIONS START **//
__attribute__((noreturn)) static void priv_error_state();
static int priv_setup_peripherals();
static int priv_setup_clock();
//** PRIVATE FUNCTIONS END **//

//** PUBLIC FUNCTIONS START **//
void HAL_Delay(uint32_t delay_ms);
volatile uint32_t HAL_getSysTick();
uint32_t HAL_getElapsed(uint32_t start);
//** PUBLIC FUNCTIONS END **//

extern int app_entry(HydrUDriver_t*);

__attribute__((noreturn))
static void priv_error_state(){
	glb_errorstate = true;
	while(1){
		asm volatile("nop");
	}
}


static int priv_setup_peripherals(){

	//setup timer
	uint32_t perclk = hal_clkmgh_get_peripheral_freq();
	hal_apb_config_t apbtmr;
	apbtmr.cascode = HAL_APBTMR_TMR_INDIPENDET;
	apbtmr.clksrc = HAL_APBTMR_CLKSRC_SYSCLK;
	apbtmr.mode = HAL_APBTMR_MODE_CONT;
	apbtmr.precsaler_en = 1;
	apbtmr.prescaler = 99;
	apbtmr.tmrhilo = HAL_APBTMR_TMR_HI;
	apbtmr.compare = (perclk /(100*SYSTEN_TICK_FREQ_HZ))-1;
	hal_apbtmr_config(APB_TMR, apbtmr);
	hal_apbtmr_int_pending_clear(APB_TMR, HAL_APBTMR_TMR_HI);
	hal_apbtmr_int(APB_TMR, HAL_APBTMR_TMR_HI, 1);
	hal_apbtmr_start(APB_TMR, HAL_APBTMR_TMR_HI);

	return 0;
}

static int priv_setup_clock(){
	if (hal_clkmgh_set_sysclk_div(CLKMGH_SYSCLK_DIV_16)){
		return -1;
	}
	if (hal_clkmgh_set_perclk_div(CLKMGH_PERCLKDIV_2)){
		return -1;
	}
	if (hal_clkmgh_set_cpuclk_src(CLKMGH_CPUCLK_SRC_SYSCLK)){
		return -1;
	}
	return 0;
}


//X Hydrogen driver, call HydrUDrv_init, set Callbacks pDelay and pgetSystick, HydrUDrv_configure
int
main(void)
{
	//init sistem
	if (hal_system_init(SYSTEM_XTAL_CLOCK_FREQ_HZ, SYSTEM_PLL_FREQ_MHZ)){
		priv_error_state();
	}


	if (priv_setup_clock()){
		priv_error_state();
	}

	if (priv_setup_peripherals()){
		priv_error_state();
	}
	//enable interrupts (fifo and timer Hi)
	hal_spr_read_then_set(CSR_MTVEC, 1); //enabe interrupt vector mode
	hal_spr_read_then_set(CSR_MSTATUS, (1<<3)); //enable interrupts
	hal_intc_enable(ITC_TIMER_HI_ID);


	//setup IPC
	ipc_init();

	//start co-processor startup
	hal_copctl_set_bootaddress((uint32_t)&__copr_start);
	hal_copctl_rst_deassert();

	//init handler
	if (HydrUDrv_init(&HydrHandler))
		priv_error_state();

	//initialize function pointers
	HydrHandler.pDelay = HAL_Delay;
	HydrHandler.pGetSysTick = HAL_getSysTick;

	//configure default
	if (HydrUDrv_configure(&HydrHandler, SYSTEM_XTAL_CLOCK_FREQ_HZ, DEF_FB_TX, DEF_FB_RX))
		priv_error_state();
	app_entry(&HydrHandler);

  return 0;
}


void HAL_Delay(uint32_t delay_ms){
	uint32_t startTime;
	for(;;){
		if((HAL_getSysTick()-startTime) > delay_ms){
			break;
		}
	}
}

volatile uint32_t HAL_getSysTick(){
	return glb_sysTick;
}

uint32_t HAL_getElapsed(uint32_t start){
	return (HAL_getSysTick()-start);
}


__attribute__((interrupt("machine")))
void isr_timer_hi(){
	//hal_apbtmr_int_pending_clear(APB_TMR, HAL_APBTMR_TMR_HI);
	APB_TMR->IRQRST_HI = (1 << APB_TMR_IRQRST_BIT);
	glb_sysTick++;
};


