/*! ----------------------------------------------------------------------------
 *  @file    bootloader_platform.h
 *  @brief   In order to update STM32 firmware via bootloader. I2C + Boot +Reset Pins and fucntions 
 *           need to be defined 
 *            
 * @attention
 *
 * Copyright 
 *
 * All rights reserved.
 *
 * @author Kai Zhao
 */
#ifndef _BOOTLOADER_PLATFORM_H_
#define _BOOTLOADER_PLATFORM_H_

/*

******This platform file works for ESP32 arduino.******
	

*/
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif


#define   BOOTLOADER_I2C           0
#define   BOOTLOADER_UART          1

/***** choose bootloader type here  *****/
#define BOOTLOADER_PORT        BOOTLOADER_UART       

#define ENABLE_DEBUG_LOG 

#if (BOOTLOADER_PORT==BOOTLOADER_I2C )
  #define DEVICE_IIC_ADDRESS          0x48  //check STM32 AN2606 for the IIC address for eath device model 
#elif  (BOOTLOADER_PORT==BOOTLOADER_UART )
  
#endif


#define ACK                                 0x79
#define NACK                                0x1F
#define BUZY                                0x76

typedef enum pRESULT{
	RES_OK,
	RES_FAIL
} pRESULT;

#ifdef ENABLE_DEBUG_LOG 
void LogDebugInfo(const char * log );
void LogDebugInfoHEX(const uint8_t   log );
#endif

/**
  * @brief  Open Master I2C port / UART port
  * @retval result 0 = RES_OK  1 = RES_FAIL
  */
pRESULT platform_init(void);

/**
  * @brief  Close Master I2C port / UART port
  * @retval result 0 = RES_OK  1 = RES_FAIL
  */
pRESULT platform_deinit(void);
///**
//  * @brief  Receive in Master I2C mode/ UART
//  * @param  reg       7 bit I2C address, 0x0xxx xxxx
//  * @param  bufp      buffer to receive data
//  * @param  len       data length
//  * @retval result 0 = RES_OK  -1 = RES_FAIL
//  */
//pRESULT platform_read( uint8_t *bufp, uint16_t len);

/**
  * @brief  Receive in Master I2C mode/ UART  with timeout
  * @param  reg       7 bit I2C address, 0x0xxx xxxx
  * @param  bufp      buffer to receive data
  * @param  len       data length
  * @param  timeout   in milisecond
  * @retval result 0 = RES_OK  -1 = RES_FAIL
  */
pRESULT platform_read_with_timeout( uint8_t *bufp, uint16_t len,int timeout );
/**
  * @brief  Transmit in Master I2C mode  / UART
  * @param  reg       7 bit I2C address, 0x0xxx xxxx
  * @param  bufp      data buffer to send
  * @param  len       data length
  * @retval result 0 = RES_OK  1 = RES_FAIL
  */
pRESULT platform_write( uint8_t *bufp, uint16_t len);

/**
  * @brief  Open Reset  BOOT0 GPIO ports to output mode
  * @retval result 0 = RES_OK  1 = RES_FAIL
  */
pRESULT platform_gpio_init(void);

/**
  * @brief  Close Reset  BOOT0 GPIO ports
  * @retval result 0 = RES_OK  1 = RES_FAIL
  */
pRESULT platform_gpio_deinit(void);

/**
  * @brief  Set Reset PIN value
  * @retval 
  */
void platform_configure_reset_pin(uint8_t value);

/**
  * @brief  Set BOOT0 PIN value
  * @retval 
  */
void platform_configure_boot0_pin(uint8_t value);

/**
  * @brief  delay some time in milliseconds 
  * @param  time_ms 
  * @retval 
  */
void platform_delay_ms(uint32_t time_ms);


#ifdef __cplusplus
}
#endif

#endif //_BOOTLOADER_PLATFORM_H_
