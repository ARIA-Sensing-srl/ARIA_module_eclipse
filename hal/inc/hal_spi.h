/**
 * @file hal_spi.h
 * @brief master SPI HAL
 *
 *  Created on: May 31, 2024\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef HAL_INC_HAL_SPI_H_
#define HAL_INC_HAL_SPI_H_




	#include <stdbool.h>
	#include "udma.h"

#define HAL_SPI_CH0 0
#define HAL_SPI_CH1 1

#define HAL_SPI_CS0 0
#define HAL_SPI_CS1 1
#define HAL_SPI_CS2 2
#define HAL_SPI_CS3 3

#define HAL_QSPI_TRANSACTION_RD 0
#define HAL_QSPI_TRANSACTION_WR 1

typedef int hal_spi_status;

/**
 * @struct _hal_spi_t
 * SPI handler, filled by the init function
 */
typedef  struct _hal_spi_t {
	volatile uDMA_SPI_t * handler;
	uint32_t channel;
	uint32_t cfg;
}hal_spi_t;

/**
 * @struct _hal_spi_config
 * SPI channel configuration structure
 * @var _hal_spi_config::channel
 * selected channel HAL_SPI_CHx
 * @var _hal_spi_config::baudrate
 * Clock rate divieder
 * @var _hal_spi_config::cpol
 * Clock polarity
 * @var _hal_spi_config::cpha
 * Clock phase
 */
typedef struct _hal_spi_config {
	uint8_t channel;
	uint32_t baudrate;
	uint8_t cpol;
	uint8_t cpha;
}hal_spi_config;
/**
 * @struct _hal_qspi_transaction_t
 * QuadSPI transaction configuration structure
 * @var _hal_qspi_transaction_t::transactionType
 * Direction (read or write)
 * @var _hal_qspi_transaction_t::cmdSize
 * Size of the command in bytes
 * @var _hal_qspi_transaction_t::cmd
 * Command to send on command phase
 * @var _hal_qspi_transaction_t::cmdQPIMode
 * Enable quadSPI on command phase (otherwise single)
 * @var _hal_qspi_transaction_t::dummyCycles
 * Number of dummy cycles
 * @var _hal_qspi_transaction_t::IOQPIMode
 * Enable QuadSPI mode on data phase
 *
 */
typedef struct _hal_qspi_transaction_t {
	uint8_t transactionType;
	uint8_t cmdSize;
	uint8_t cmd;
	uint8_t cmdQPIMode;
	uint8_t dummyCycles;
	uint8_t IOQPIMode;
}hal_qspi_transaction_t;

/**
 * Initialize the selected channel
 * @param ph channel handler
 * @param pconfig configuration
 * @return SUCCESS/FAIL
 */
hal_spi_status hal_spi_init(hal_spi_t* ph,hal_spi_config * pconfig);
/**
 * Perform blocking full-duplex SPI transaction
 * @param ph channel hanlder
 * @param pwrite pointer to output buffer
 * @param pread pointer to input buffer
 * @param size number of bytes to transfer
 * @param chipselect selected chipselect channel HAL_SPI_CSx
 * @return SUCCESS/FAIL
 */
hal_spi_status hal_spi_fullduplex(hal_spi_t*ph, uint8_t * pwrite, uint8_t * pread, int size, uint8_t chipselect);
/**
 * De-initialize the selected channel
 * @param ph
 * @return SUCCESS/FAIL
 */
hal_spi_status hal_spi_deinit(hal_spi_t* ph);
/**
 * Perform a QuadSPI transaction
 * @param ph channel handler
 * @param transConfig configuration structure for current transaction
 * @param pbuffer data buffer
 * @param size numbr of bytes to transfer
 * @param chipselect selected chipselect channel HAL_SPI_CSx
 * @return SUCCESS/FAIL
 */
hal_spi_status hal_spi_QSPITransaction(hal_spi_t*ph, hal_qspi_transaction_t* transConfig, uint8_t * pbuffer, int size, int chipselect);



#define HAL_SPI_SUCCESS 0
#define HAL_SPI_ERROR -1
#define HAL_SPI_INVALID_PARAM -2



#endif /* HAL_INC_HAL_SPI_H_ */
