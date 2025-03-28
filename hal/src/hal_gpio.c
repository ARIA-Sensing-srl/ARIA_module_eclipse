/*
 * hal_gpio.c
 *
 *  Created on: May 31, 2024
 *      Author: andream
 */


#include "hal_gpio.h"
#include "socctrl.h"
#include "hal_gpio_alt_map.h"


static int priv_setAlt(uint32_t pinnum, uint32_t altMode);
static int priv_setCfgPinAsGPIO(uint32_t pinnum);


hal_gpio_result hal_gpio_configure(volatile gpio_t * p, hal_gpio_config_t * pcfg){
	p->GPIOEN = (1 << pcfg->pinnum);
	switch (pcfg->mode){
		case GPIO_MODE_INPUT:
			p->PADDIR &= ~(1 << pcfg->pinnum); //disable pad
			priv_setCfgPinAsGPIO(pcfg->pinnum);
			break;
		case GPIO_MODE_OUTPUT:
			hal_gpio_set_level(p, pcfg->pinnum, pcfg->defLvl);
			p->PADDIR |= (1 << pcfg->pinnum); //enable pad
			priv_setCfgPinAsGPIO(pcfg->pinnum);
			break;
		case GPIO_MODE_ALT:
			priv_setAlt(pcfg->pinnum, pcfg->alt);
			break;
		default:
			return HAL_GPIO_FAIL;
			break;
	}
	return HAL_GPIO_SUCCESS;
}
hal_gpio_result hal_gpio_set_level(volatile gpio_t * p, uint32_t pin, hal_gpio_level_t lvl){
	if (lvl == GPIO_LVL_RESET)
		p->PADOUTCLR = (1 << pin);
	else
		p->PADOUTSET = (1 << pin);
	return 0;
}
hal_gpio_level_t hal_gpio_get_level(volatile gpio_t * p, uint32_t pin){
	if (p->PADIN & (1 << pin))
		return GPIO_LVL_SET;
	return GPIO_LVL_RESET;
}


static int priv_setAlt( uint32_t pinnum, uint32_t altMode){
	int padmux;
	padmux = SOCCTRL_PADCTRL->PADMUX[0];
	padmux &= ~(3 << 2*pinnum);
	padmux |= ((altMode & 0x3) << pinnum*2);
	SOCCTRL_PADCTRL->PADMUX[0] = padmux;
	return 0;
}

//set PAD as GPIO
static int priv_setCfgPinAsGPIO( uint32_t pinnum){
	uint32_t alt;
	switch (pinnum){
		case GPIO_PIN0:
			alt = GPIO0_ALT_GPIO;
			break;
		case GPIO_PIN1:
			alt = GPIO1_ALT_GPIO;
			break;
		case GPIO_PIN2:
			alt = GPIO2_ALT_GPIO;
			break;
		case GPIO_PIN3:
			alt = GPIO3_ALT_GPIO;
			break;
		case GPIO_PIN4:
			alt = GPIO4_ALT_GPIO;
			break;
		case GPIO_PIN5:
			alt = GPIO5_ALT_GPIO;
			break;
		case GPIO_PIN6:
			alt = GPIO6_ALT_GPIO;
			break;
		case GPIO_PIN7:
			alt = GPIO7_ALT_GPIO;
			break;
		case GPIO_PIN8:
			alt = GPIO8_ALT_GPIO;
			break;
		case GPIO_PIN9:
			alt = GPIO9_ALT_GPIO;
			break;
		case GPIO_PIN10:
			alt = GPIO10_ALT_GPIO;
			break;
		case GPIO_PIN11:
			alt = GPIO11_ALT_GPIO;
			break;
		case GPIO_PIN12:
			alt = GPIO12_ALT_GPIO;
			break;
		case GPIO_PIN13:
			alt = GPIO13_ALT_GPIO;
			break;
		default:
			break;
	}
	return priv_setAlt(pinnum, alt);
}

hal_gpio_result hal_gpio_int_confiure(gpio_t*p, uint32_t pin, hal_gpio_int_t type){
	uint32_t inttype;
	inttype = p->INTTYPE[0];
	inttype &= ~(3 << (2*pin));
	inttype |= (((uint32_t)type & 3) < (2*pin));
	p->INTTYPE[0] = inttype;
	return 0;
}
hal_gpio_result hal_gpio_int_enable(gpio_t*p, uint32_t pin){
	p->INTEN |= (1 << pin);
	return 0;
}
hal_gpio_result hal_gpio_int_disable(gpio_t*p, uint32_t pin){
	p->INTEN &= ~(1 << pin);
	return 0;
}

uint32_t hal_gpio_get_intstatus_and_clear(gpio_t*p){
	return p->INTSTATUS;
}
