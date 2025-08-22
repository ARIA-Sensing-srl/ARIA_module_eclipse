/**
 * @file NVM.h
 * @brief Not volatile memory storage manager
 *
 * NVM handles access to not volatile memory storage inside the flash\n
 * The last block of the flash (4096 byte) is used fo NVM storage\n
 * Data are stored and reference with unique strings\n
 *
 *
 *  Created on: 19/ago/2025\n
 *  Author: ARIA Sensing\n
 *  Copyright: ARIA Sensing, 2023 - \n
 */

#ifndef NVM_H_
#define NVM_H_

typedef enum _NVMTypes_t{
	NVM_8=0,
	NVM_16=1,
	NVM_32=2,
}NVMTypes_t;

#define NVM_MAX_KEY_LENGTH 8

/**
 * @brief setup peripheral for flash access
 *
 * This function initializes SPI communication and verifies flash type and size
 *
 * @return SUCCESS or FAIL
 */
int NVM_init();

/**
 * @brief Read data from NVM
 *
 * This function search for the given key and return data (if exists)
 *
 * @param key
 * @param p
 * @return error or number of bytes
 */
int NVM_read(char * key, void * p);

/**
 * @brief write data to not volatile storage
 *
 * This function store the data according to given key, if key exists the function replace it
 *
 * @param key string
 * @param type size of a single item
 * @param size number of item to store
 * @param p pointer to data memory
 * @return SUCCESS or FAIL
 */
int NVM_write(char * key, NVMTypes_t, int size ,void * p);

/**
 * @brief clear NVM entry
 *
 * @param key
 * @return SUCCESS or FAIL
 */
int NVM_erase(char * key);

/**
 * @brief open NVM
 *
 * This function open the storage and verify data integrity
 *
 * @return SUCCESS or FAIL
 */
int NVM_open();

/**
 * @brief close NVM
 *
 * This function close NVM access and write pending data to external flash
 * Before this function is called, data are stored into RAM
 *
 * @return SUCCESS or FAIL
 */
int NVM_close();

/**
 * @brief format flash for NVM usage
 *
 * This function format the flash section to be used for NVM purpose, this operation clear all the existing content
 *
 * @return SUCCESS or FAIL
 */
int NVM_format();


#define NVM_SUCCESS 0
#define NVM_ERROR -1
#define NVM_ERROR_KEY_LENGTH -2
#define NVM_ERROR_INTEGRITY -3
#define NVM_ERROR_KEY_NOT_FOUND -4
#define NVM_ERROR_CLOSE -5
#define NVM_ERROR_TOO_LARGE -6



#endif /* NVM_H_ */
