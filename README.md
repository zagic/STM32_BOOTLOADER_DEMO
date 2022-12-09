# A STM32 bootloader OTA demo
This demo is to update the firmware of a STM32 MCU using STM32 bootloader. 
It has been verified with a STM32L432 MCU. The protocol of different STM32 series of MCU may be slightly different. Therefore, the code needs to be modified if the target device is different.
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
Select either UART or I2C in "bootloader_platform.h" by defining BOOTLOADER_PORT  to either BOOTLOADER_UART  or BOOTLOADER_I2C.
In order to run this demo,  you need to build a STM32 firmware and save it to a **HTTP server** (is not included in this repo) and generate a download URL for it. 
>Copy that URL to credetials.h
Copy the Wi-Fi SSID and password to credetials.h

If you do not have a URL for the firmware, you need to define "DUMMY_TEST" and can only do a local test with a dummy firmware.

# Noticeable issues
The bootloader of STM32 is quit sensetive. Here are some findings of the STM32 behaviour that might not be mentioned in their document. The following behaviour was observed on STM32L432. It may vary from differnet chipsets. However, if the bootloader does not work, the following clue might be enlightening.

When using UART1, the commands and data need to be sent byte by byte (with 1ms interval). It would not work if sending them continuously (even with 9600 baudrate).
When using I2C1, the commands and data need to be sent in time. In our test, if the parameter sent 1s after a command (e.g. erase memory), a 0x1F is returned.
Arduino I2C buffer length is 32, therefore, you have no way to read data longer than 32 bytes at a time (unless you change the Arduino lib). Even the maximum reading sector size of STM32 bootloader is 256, we just read 16 or 32 at a time when using I2C. 
In our test, writting 128 or more bytes via I2C will also cause error. Therefore, we set it to write 64 bytes at a time.
Other issues:
When erasing STM32L432 via I2C bootloader, the last ACK is not returned. If master device (Arduino ESP32) requests multiple times, a 0x1F is returned (but the erase process successed). This 0x1F might be ignored. 
The GND of master device and STM32 slave device is suggested to be directly connected. The IAP may fail even they both connect to the same PC via USB.