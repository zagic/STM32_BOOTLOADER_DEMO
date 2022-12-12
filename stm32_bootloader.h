/*! ----------------------------------------------------------------------------
 *  @file    stm32_i2c_bootloader.h
 *  @brief   
 *
 * @attention
 *
 * Copyright 
 *
 * All rights reserved.
 *
 * @author Kai Zhao
 */
#ifndef _STM32_I2C_BOOTLOADER_H_
#define _STM32_I2C_BOOTLOADER_H_

#include "bootloader_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SLAVE_SERIAL_TIMEOUT                     15000
#define SLAVE_SERIAL_TIMEOUT_ERASE               120000



/**
  * @brief  flash data to STM32
  * @param  address     flash address 
  * @param  data        data to write
  * @param  len         data length, no greater than 256
  * @retval result 0 = RES_OK  1 = RES_FAIL
  */
pRESULT flashSlavePage(const uint8_t *address, const uint8_t *data,uint16_t len);

/**
  * @brief  compare the data to the bytes read from STM32 
  * @param  address     flash address 
  * @param  data        data to compare
  * @retval result 0 = RES_OK  1 = RES_FAIL
  */
pRESULT verifySlavePage(const uint8_t *address, const uint8_t *data,uint16_t len);


/**
* @brief  read and print some useful information,erase sectors. You may adjust the sectors num based your firmware size
* @retval RES_OK or RES_FAIL
*/
pRESULT checkAndEraseSTM(void);

/**
* @brief tongle pins to start bootloader
*/
void startBootloader(void);
/**
* @brief tongle reset pin to restart device
*/
void resetSTM(void);

/**
* @brief tongle pins to end bootloader and start program
*/
void endBootloader(void);

#if (BOOTLOADER_PORT==BOOTLOADER_UART)
pRESULT bootloaderSync(void);
#endif
pRESULT bootloaderGet(void);
pRESULT bootloaderVersion(void);
pRESULT bootloaderId(void);
pRESULT bootloaderWrite(void);
pRESULT bootloaderRead(void);
pRESULT bootloaderReleaseMemProtect(void);
pRESULT bootloaderExtErase(void);//not verified
 /**
  * @brief Erase continuous sectors, in STM32L432, sectors are all in 2K, 
  * @param  startPageIdx          start sector/page index
  * @param  pageNum               page/sector num in total (recomend to erase 1-4 sectors at a time, have not verified more sectors)
  * @retval RES_OK or RES_FAIL
  */
pRESULT bootloaderErasePages(uint16_t startPageIdx, uint16_t pageNum);






#ifdef __cplusplus
}
#endif

#endif //_STM32_I2C_BOOTLOADER_H_
