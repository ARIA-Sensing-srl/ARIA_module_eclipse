/**
 * @file hal_gpio.h
 * @brief General purpose IO HAL
 *
 *  Created on: May 31, 2024\n
 * 	Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_INC_HAL_GPIO_H_
#define HAL_INC_HAL_GPIO_H_

#include "gpio.h"

/**
 * GPIO IO control mode selection
 */
typedef enum _hal_gpio_mode_t {
	GPIO_MODE_INPUT = 0, /**< GPIO_MODE_INPUT */
	GPIO_MODE_OUTPUT = 1,/**< GPIO_MODE_OUTPUT */
	GPIO_MODE_ALT = 2    /**< GPIO_MODE_ALT */
}hal_gpio_mode_t;
/**
 * GPIO mux selection
 */
typedef enum _hal_gpio_alt_t {
	GPIO_ALT0 = 0,/**< GPIO_ALT0 */
	GPIO_ALT1 = 1,/**< GPIO_ALT1 */
	GPIO_ALT2 = 2,/**< GPIO_ALT2 */
	GPIO_ALT3 = 3 /**< GPIO_ALT3 */
}hal_gpio_alt_t;
/**
 * GPIO IO level definition
 */
typedef enum _hal_gpio_level_t {
	GPIO_LVL_RESET = 0,/**< GPIO_LVL_RESET */
	GPIO_LVL_SET = 1   /**< GPIO_LVL_SET */
}hal_gpio_level_t;
/**
 * GPIO interrupt edge configuration
 */
typedef enum _hal_gpio_int_t {
	GPIO_INT_FALLING = 0,/**< GPIO_INT_FALLING */
	GPIO_INT_RISING = 1, /**< GPIO_INT_RISING */
	GPIO_INT_BOTH        /**< GPIO_INT_BOTH */
}hal_gpio_int_t;

typedef int hal_gpio_result;

/**
 * @struct _hal_gpio_config_t
 * GPIO configuration structure
 * @var _hal_gpio_config_t::pinnum
 * GPIO pin
 * @var _hal_gpio_config_t::mode
 * GPIO operating mode
 * @var _hal_gpio_config_t::alt
 * GPIO mux selection
 * @var _hal_gpio_config_t::defLvl
 * GPIO default output level (when controlled by GPIO peripheral)
 */
typedef struct _hal_gpio_config_t {
	int pinnum;
	hal_gpio_mode_t mode;
	hal_gpio_alt_t alt;
	hal_gpio_level_t defLvl;
}hal_gpio_config_t;

/**
 * Configure GPIO pin
 * @param p handler
 * @param pcfg pointer to configuration structure
 * @return SUCCESS/FAIL
 */
hal_gpio_result hal_gpio_configure(volatile gpio_t * p, hal_gpio_config_t * pcfg);
/**
 * Set GPIO output level
 * @param p handler
 * @param pin pad number
 * @param lvl output level
 * @return SUCCESS/FAIL
 */
hal_gpio_result hal_gpio_set_level(volatile gpio_t * p, uint32_t pin, hal_gpio_level_t lvl);
/**
 * Get GPIO input level
 * @param p handler
 * @param pin pad number
 * @return SUCCESS/FAIL
 */
hal_gpio_level_t hal_gpio_get_level(volatile gpio_t * p, uint32_t pin);
/**
 * Configure interrupt generation
 * @param p handler
 * @param pin pad number
 * @param interrupt edge mode
 * @return SUCCESS/FAIL
 */
hal_gpio_result hal_gpio_int_confiure(gpio_t*p, uint32_t pin, hal_gpio_int_t interrupt);
/**
 * Enable interrupt on selected pin
 * @param p handler
 * @param pin pad number
 * @return SUCCESS/FAIL
 */
hal_gpio_result hal_gpio_int_enable(gpio_t*p, uint32_t pin);
/**
 * Disable interrupt on selected pin
 * @param p handler
 * @param pin pad number
 * @return SUCCESS/FAIL
 */
hal_gpio_result hal_gpio_int_disable(gpio_t*p, uint32_t pin);
/**
 * Read the interrupt status register and clear the register
 * @param p
 * @return interrupt status mask
 */
uint32_t hal_gpio_get_intstatus_and_clear(gpio_t*p);


#define HAL_GPIO_SUCCESS 0
#define HAL_GPIO_FAIL -1


#endif /* HAL_INC_HAL_GPIO_H_ */
