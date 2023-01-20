# A STM32 bootloader OTA demo

This demo is to update the firmware of an STM32 MCU using the STM32 bootloader. 

It has been verified with an STM32L432 MCU. The protocol of different STM32 series of MCU may be slightly different. Therefore, the code needs to be modified if the target device is different.
The master device in this demo is ESP32 with the sample code written in Arduino IDE. 
The sample support both IIC and UART port. Please refer to the following two links for the protocols of IIC and UART bootloader protocol.

 [Link1](https://www.st.com/resource/en/application_note/an4221-i2c-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf)   (an4221-i2c-protocol-used-in-the-stm32-bootloader-stmicroelectronics)
 
 [Link2](https://www.st.com/resource/en/application_note/an3155-usart-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf)  (an3155-usart-protocol-used-in-the-stm32-bootloader-stmicroelectronics)

## Run the demo
Connect the ESP32 and STM32 boards:

ESP32 (Arduino)===============STM32L432

PIN13 -----------------------------------Reset

PIN14 -----------------------------------Boot0

I2C

I2C SDA  (PIN21)---------------------SDA PB7  (Please refer to [Link3](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf) for the PIN allocation )

I2C CLK  (PIN22)---------------------CLK  PB6 (Please refer to [Link3](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf) for the PIN allocation )

or UART

UART TX (PIN21)--------------------- UART RX PA10  (Please refer to [Link3](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf) for the PIN allocation )

UART RX (PIN22)--------------------- UART TX PA9 (Please refer to [Link3](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf) for the PIN allocation )

Open the project from Arduino IDE, and choose ESP32 dev module (or the module you have). Choose a partition scheme with SPIFFS (For me, it is default 4MB flash with SPIFFS)

Select either UART or I2C in "bootloader_platform.h" by defining BOOTLOADER_PORT  to either BOOTLOADER_UART  or BOOTLOADER_I2C.
In order to run this demo,  you need to build an STM32 firmware and save it to an **HTTP server** (which is not included in this repo) and generate a download URL for it. 
>Copy that URL to credentials.h
>Copy the Wi-Fi SSID and password to credentials.h

If you do not have a URL for the firmware, you need to define "DUMMY_TEST" and can only do a local test with a dummy firmware.

# Noticeable issues
The bootloader of STM32 is quite sensitive. Here are some findings of the STM32 behaviour that might not be mentioned in their document. The following behaviour was observed on STM32L432. It may vary from different chipsets. However, if the bootloader does not work, the following clue might be enlightening.

>When using UART1, the commands and data need to be sent byte by byte (with a 1ms interval). It would not work if sending them continuously (even with 9600 baudrate).

>When using I2C1, the commands and data need to be sent in time. In our test, if the parameter sent 1s after a command (e.g. erase memory), a 0x1F is returned.

>Arduino I2C buffer length is 32, therefore, you have no way to read data longer than 32 bytes at a time (unless you change the Arduino lib). Even though the maximum reading sector size of STM32 bootloader is 256, we just read 16 or 32 at a time when using I2C. 

>In our test, writing 128 or more bytes via I2C will also cause an error. Therefore, we set it to write 64 bytes at a time.

Other issues:

>When erasing STM32L432 via I2C bootloader, the last ACK is not returned. If master device (Arduino ESP32) requests multiple times, a 0x1F is returned (but the erase process succeeded). This 0x1F might be ignored. 

>The GND of master device and STM32 slave device is suggested to be directly connected. The IAP may fail even if they both connect to the same PC via USB.

>Right now, the code only erase the first a few sectors of the STM32 flash. Customize how much size of the flash need to be erased in stm32_bootloader.c/pRESULT checkAndEraseSTM(void) 

# Bootloader protocol
In our test, 

Target board = STM32L432

I2C bootloader version = 0x12

 UART bootloader version = 0x31

The bootloader protocol might need to be slightly modified if the board or version is different.

# Transplant code across different platforms
This sample has been verified on the ESP32 Arduino environment. To run it on a different platform, *stm32_bootloader.c* and *stm32_bootloader.h* can be reused. You need to rewrite the method in *bootloader_platform.c* based on your new platform. In addition, you also need to prepare the firmware in your code and rewrite method *readSlaveFlashandVerify()* and *writeFlash()*.
