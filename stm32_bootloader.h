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

pRESULT checkAndEraseSTM(void);



void endBootloader(void);

#ifdef __cplusplus
}
#endif

#endif //_STM32_I2C_BOOTLOADER_H_
