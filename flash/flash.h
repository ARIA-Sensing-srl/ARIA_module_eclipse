/**
 * @file flash.h
 * @brief Flash handler
 *
 * This groups of functions handle access to the external SPI memory\n
 *
 *
 *
 *  Created on: 19/ago/2025\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef FLASH_H
#define FLASH_H
#include "hal_spi.h"
#include "stdbool.h"

#define FL_SPI_CHANNEL 	HAL_SPI_CH0
#define FL_SPI_CS_NUM 	HAL_SPI_CS0

#define FL_BOOT_NUM_SECTORS 6

#define FL_BLOCK32_SIZE 	32768
#define FL_BLOCK64_SIZE 	65536
#define FL_SECTOR_SIZE		4096
#define FL_PAGE_SIZE		256

/**
 * @brief flash initialization
 *
 * This procedure initialize the SPI interface and identify the flash\n
 *
 * @param clkfreq_Hz
 * @return SUCCESS or FAIL
 */
int FL_init(uint32_t clkfreq_Hz);

/**
 * @brief Deinitialize the interface and disable the SPI interface
 * @return SUCCESS or FAIL
 */
int FL_deinit();

/**
 * @brief erase one sector
 * The erased sector is aligned to sector boundary (the function align the address to the sector)\n
 * @param sectorAddress
 * @return SUCCESS or FAIL
 */
int FL_erasesector(uint32_t sectorAddress);

/**
 * @brief Erase one 32 Kbyte block
 *
 * This function erase one 32 kbyte block, block address will be aligned to block size
 * @param blockAddress
 * @return SUCCESS or FAIL
 */
int FL_eraseblock32(uint32_t blockAddress);

/**
 * @brief read a block of data from flash
 * @param address is the flash start address
 * @param pdest destination memory pointer
 * @param size number of bytes
 * @return SUCCESS or FAIL
 */
int FL_read(uint32_t address, uint8_t* pdest, int size);

/**
 * @brief write data into memory
 * @param address, destination flash address
 * @param psrc data source array
 * @param size number of bytes
 * @return SUCCESS or FAIL
 */
int FL_write(uint32_t address, uint8_t* psrc, int size);

/**
 * @brief Check if specified flash's part is blank
 * @param startaddress
 * @param size
 * @return true if blank
 */
bool FL_isblank(uint32_t startaddress, uint32_t size);

/**
 * @brief Execute mass erase of the flash
 *
 * This operation clear the overall flash content. Depending on flash size, this operation could take
 * several seconds.
 * The first FL_BOOT_NUM_SECTORS sectors are kept.
 * @return SUCCESS or FAIL
 */
int FL_masserase();

/**
 * @brief Return the flash size in bytes
 *
 * @return flash size in byte
 */
uint32_t FL_getFlashSize();

#define FL_SUCCESS 0
#define FL_ERROR -1

#endif



