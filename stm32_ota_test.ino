#include "SPIFFS.h"
#include "FS.h"
#include <HTTPClient.h>
#include <WiFi.h>

/*
 * To run this demo, you need to create a head file "credentials.h" where your WiFi SSID and password and the url of your STM32 firmware are defined 
 * Otherwise, please define DUMMY_TEST, which would test with a local dummy firmware
 *  credentials.h template:
 *  
 *      #ifndef __CREDENTIALS_H
 *      #define __CREDENTIALS_H
 *      
 *      #define WIFI_SSID                          "XXX"
 *      #define WIFI_PASSWORD                      "XXXXXXXX"
 *      #define STM32_FIRMWARE_HTTP_URL            "https://xxxxxxxxxxx"
 *      
 *      #endif  //__CREDENTIALS_H
*/
//#define DUMMY_TEST

#ifndef DUMMY_TEST
#include "credentials.h"
#endif

#include "stm32_bootloader.h"

#define FILE_PATH_NAME                          "/spiffs/STM32L4.bin"

/****it is tested that the maximum length is 126 for STM32L4 I2C ****/
#if (BOOTLOADER_PORT==BOOTLOADER_I2C)
  #define MAX_WRITE_BLOCK_SIZE                    64   
  #define MAX_READ_BLOCK_SIZE                     16             // this is because of the Arduino IIC buffer len is 32,
#else
  #define MAX_WRITE_BLOCK_SIZE                    256
  #define MAX_READ_BLOCK_SIZE                     256
#endif

File file;           //file to save stm32 firmware

void initSPIFFS(void);
#ifndef DUMMY_TEST
void downloadFromHTTP(void);
void wifiConnect(void);
#endif
void flashSTM32(void);
void verifySTM32(void);

void testSPIFFS(void);
pRESULT writeFlash(void);
pRESULT readSlaveFlashandVerify(void);

void setup() {
  Serial.begin(115200);

  platform_init();
  initSPIFFS();
  
#ifndef DUMMY_TEST
  wifiConnect();
  downloadFromHTTP();
#else
  testSPIFFS();
#endif
  checkAndEraseSTM();
  writeFlash();
  readSlaveFlashandVerify();
  endBootloader();
  
  Serial.println("Demo finhish");
}

void loop() {
  // put your main code here, to run repeatedly:

}
#ifndef DUMMY_TEST
void wifiConnect(void){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void downloadFromHTTP(void){

 if(SPIFFS.exists(FILE_PATH_NAME)){ //delete old file if exist 
  SPIFFS.remove(FILE_PATH_NAME);
 }
 delay(100);
 file = SPIFFS.open(FILE_PATH_NAME, "a");

 if (!file) {
   Serial.println("- failed to open file for writing");
   return;
 }

 if ((WiFi.status() == WL_CONNECTED)) { // Check the current connection status
    HTTPClient http;
    http.begin(STM32_FIRMWARE_HTTP_URL); //Specify the URL and certificate
    int httpCode = http.GET(); // Make the request
    
    if (httpCode == HTTP_CODE_OK) { // Check for the returning code
      int len = http.getSize();
      Serial.printf("download size : %d",len);
      // create buffer for read
      uint8_t buff[512] = { 0 };
      
      // get tcp stream
      WiFiClient * stream = http.getStreamPtr();
      
      // read all data from server
      while(http.connected() && (len > 0 || len == -1)) {
          // get available data size
          size_t size = stream->available();
      
          if(size) {
              // read up to 128 byte
              int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
      
              // write it to Serial
              file.write(buff,c);
              //Serial.printf("write %d bytes",c);
              if(len > 0) {
                  len -= c;
              }
          }
          delay(1);
      }
      file.flush();
    }else{
      Serial.println("http get error");
      file.close();
      Serial.println("download failed");
      http.end(); // Free the resources
    }

    Serial.print(FILE_PATH_NAME); Serial.print(" size : ");Serial.println(file.size());
    file.close();
    Serial.println("download finished");
    http.end(); // Free the resources
  }else{
    file.close();
    Serial.println("no internet, download failed");
  }
  
 
  delay(1000);
}
#endif 

void initSPIFFS(void){
   delay(3000);
  Serial.println("opening SPIFFS");
  while (!SPIFFS.begin(true))
  {
    Serial.print("...");
  }
  Serial.println("SPIFFS OK!");
}



void testSPIFFS(void){
  file = SPIFFS.open(FILE_PATH_NAME, FILE_WRITE);
  if(!file){
     Serial.println("There was an error opening the file for writing");
     return;
  }else{
    if(file.print("TEST123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF")) {
      Serial.println("File was written");
    }else {
       Serial.println("File write failed");
    }
  }
  file.close();
  if(SPIFFS.exists(FILE_PATH_NAME)){
    file = SPIFFS.open(FILE_PATH_NAME, FILE_READ);
    char buf[10];
    file.readBytes(buf,10);
    Serial.println(buf);
    file.close();
  }
  Serial.printf("SPIFFS的 total size: %d bytes\r\n", SPIFFS.totalBytes());
  Serial.printf("SPIFFS的 used size: %d bytes\r\n", SPIFFS.usedBytes());
//  SPIFFS.end(); 
}


pRESULT writeFlash(void)
{

  uint8_t loadAddress[4] = {0x08, 0x00, 0x00, 0x00};
  uint8_t block[256] = {0};
  int curr_block = 0, bytes_read = 0;
  
  if(SPIFFS.exists(FILE_PATH_NAME)){
    file = SPIFFS.open(FILE_PATH_NAME, FILE_READ);
  }else{
    Serial.println("flash failed");
    return RES_FAIL;
  }

  while ((bytes_read = file.readBytes((char*)block,MAX_WRITE_BLOCK_SIZE)) > 0)
  {
    curr_block++;
    Serial.printf("Slave MCU IAP: Writing block: %d,block size: %d", curr_block,bytes_read);Serial.println("");
  
    pRESULT ret = flashSlavePage(loadAddress, block,bytes_read);
    if (ret == RES_FAIL)
    {
        return RES_FAIL;
    }
  
    memset(block, 0xff, bytes_read);
  }
  file.close();
  
  return RES_OK;
}

pRESULT readSlaveFlashandVerify(void)
{
    uint8_t readAddress[4] = {0x08, 0x00, 0x00, 0x00};
    uint8_t block[257] = {0};
    int curr_block = 0, bytes_read = 0;

    if(SPIFFS.exists(FILE_PATH_NAME)){
      file = SPIFFS.open(FILE_PATH_NAME, FILE_READ);
    }else{
      Serial.println("flash failed");
      return RES_FAIL;
    }
    while ((bytes_read = file.readBytes((char*)block,MAX_READ_BLOCK_SIZE)) > 0)
    {
        curr_block++;
        Serial.printf("Slave MCU IAP: Reading block: %d, block size: %d", curr_block,bytes_read);Serial.println("");

        pRESULT ret = verifySlavePage(readAddress, block,bytes_read);
        if (ret == RES_FAIL)
        {
            Serial.println("Data verification failed");
            file.close();
            return RES_FAIL;
        }
        memset(block, 0xff, 256);
    }
    file.close();
    Serial.println("Data verification success");
    return RES_OK;
}
