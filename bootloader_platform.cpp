/*! ----------------------------------------------------------------------------
 *  @file    bootloader_platform.c
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


#include "bootloader_platform.h"
#include <Wire.h>
 
#define MAX_ESP_ARDUINO_IIC_BUFF_LEN            32       //Arduino max IIC buffer size

int boot0Pin = 14;                                       //Arduino PIN idx
int resetPin = 13;                                       //Arduino PIN idx
#if (BOOTLOADER_PORT==BOOTLOADER_I2C)
//  int wireSLKPin = 22;                                       Arduino default PIN idx
//  int wireSDAPin = 21;                                       Arduino default PIN idx
#elif (BOOTLOADER_PORT==BOOTLOADER_UART)
  int uart1TXPin = 21;                                       //Arduino PIN idx
  int uart1RXPin = 22;                                       //Arduino PIN idx
#endif


pRESULT platform_init(void){

#if (BOOTLOADER_PORT==BOOTLOADER_I2C)
  Wire.setClock(400000); 
  Wire.begin();   
#elif (BOOTLOADER_PORT==BOOTLOADER_UART)
  Serial1.begin(57600, SERIAL_8N1, uart1RXPin , uart1TXPin );

#endif
  
	return RES_OK;
}
pRESULT platform_deinit(void){
#if (BOOTLOADER_PORT==BOOTLOADER_I2C)
 //  Serial1.end();
#elif (BOOTLOADER_PORT==BOOTLOADER_UART)
//  Wire.end();  
#endif
 
	return RES_OK;
}



pRESULT platform_read_with_timeout( uint8_t *bufp, uint16_t len,int timeout ){
  int timeEnd = millis()+timeout;
#if (BOOTLOADER_PORT==BOOTLOADER_I2C)
  Wire.requestFrom((uint16_t)DEVICE_IIC_ADDRESS, (size_t)len);
  int num =0;
  while(Wire.available()<len && (millis()< timeEnd ) ){
#ifdef ENABLE_DEBUG_LOG
      Serial.print("IIC available ");Serial.println(num);
#endif
      platform_delay_ms(100);
//      Wire.requestFrom((uint16_t)DEVICE_IIC_ADDRESS, (size_t)len);
  }
  num =Wire.available();
  if(num >=len){
    for(int i = 0;i<len;i++){
      bufp[i] = Wire.read();
    }
    return RES_OK;
  }else{
    for(int i = 0;i<num;i++){
      bufp[i] = Wire.read();
#ifdef ENABLE_DEBUG_LOG
      Serial.println(bufp[i],HEX);
#endif
    }
#ifdef ENABLE_DEBUG_LOG
    Serial.print("IIC available (less)");Serial.println(num);
#endif
    return RES_FAIL;
  }

#elif (BOOTLOADER_PORT==BOOTLOADER_UART)
  int l = 0;
  while(Serial1.available() < len && (millis()< timeEnd ) ){
    platform_delay_ms(100);
  }
  l=Serial1.available();
  if( l==len){
    Serial1.read(bufp, len);
#ifdef ENABLE_DEBUG_LOG
    Serial.println("UART receive OK ");
#endif
    return RES_OK;
  }else if(l>len){
    Serial1.read(bufp, len);
#ifdef ENABLE_DEBUG_LOG
    Serial.print("UART available (more)");Serial.println(l);
#endif
    return RES_OK;
  }else{
    Serial1.read(bufp, l);
#ifdef ENABLE_DEBUG_LOG
    Serial.print("UART available (less)");Serial.println(l);
#endif
    return RES_FAIL;
  }
#endif
  
}




pRESULT platform_write( uint8_t *bufp, uint16_t len){
#if (BOOTLOADER_PORT==BOOTLOADER_I2C)

  Wire.beginTransmission(DEVICE_IIC_ADDRESS); 
  uint16_t cursor = 0;
  while(cursor+MAX_ESP_ARDUINO_IIC_BUFF_LEN<len){
    Wire.write(bufp+cursor,MAX_ESP_ARDUINO_IIC_BUFF_LEN);
    cursor =cursor+MAX_ESP_ARDUINO_IIC_BUFF_LEN;  
  }
  if(cursor <len){
    Wire.write(bufp+cursor,len-cursor);
  }
  int res =Wire.endTransmission(); 
  if(res==0){
    return RES_OK;
  }else{
#ifdef ENABLE_DEBUG_LOG
    Serial.printf("try send %d ",len); Serial.printf("iic write errorcode %d",res);Serial.println();
#endif
    return RES_FAIL;
  }
#elif (BOOTLOADER_PORT==BOOTLOADER_UART)
  int res =0;
  for(int i = 0; i<len; i++){
    res= res + Serial1.write(bufp+i,1);
    platform_delay_ms(1);
    }
  if(res==len){
      return RES_OK;
  }else{
#ifdef ENABLE_DEBUG_LOG
    Serial.printf("uart write %d bytes",res);Serial.println();
#endif
    return RES_FAIL;
  }
#endif
	
}

pRESULT platform_gpio_init(void){
	pinMode(boot0Pin, OUTPUT);
	pinMode(resetPin, OUTPUT);
	platform_configure_reset_pin(1);
	platform_configure_boot0_pin(1);
	return RES_OK;
}

pRESULT platform_gpio_deinit(void){
	return RES_OK;
}

void platform_configure_reset_pin(uint8_t val){
	digitalWrite(resetPin, val);
}

void platform_configure_boot0_pin(uint8_t val){
	digitalWrite(boot0Pin, val);
}

void platform_delay_ms(uint32_t time_ms){
	vTaskDelay(time_ms);
}
#ifdef ENABLE_DEBUG_LOG
void LogDebugInfo(const char * log ){
	Serial.println(log);
}

void LogDebugInfoHEX(const uint8_t  log ){
  Serial.println(log,HEX);
}
#endif
