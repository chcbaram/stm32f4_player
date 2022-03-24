/*
 * hw_def.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"



#define _DEF_FIRMWATRE_VERSION    "V220323R1"
#define _DEF_BOARD_NAME           "STM32F_PLAYER"



#define _USE_HW_SD
#define _USE_HW_FATFS
#define _USE_HW_FLASH
#define _USE_HW_FILES
#define _USE_HW_RTC
#define _USE_HW_RESET



#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         3

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    24
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    4
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       5
#define      HW_BUTTON_OBJ_USE      1

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         5

#define _USE_HW_CDC
#define _USE_HW_USB
#define      HW_USE_CDC             1
#define      HW_USE_MSC             1

#define _USE_HW_SPI
#define      HW_SPI_MAX_CH          2

#define _USE_HW_LCD
#define      HW_LCD_LVGL
#define _USE_HW_ST7567
#define      HW_LCD_WIDTH           128
#define      HW_LCD_HEIGHT          64

#define _USE_HW_I2C
#define      HW_I2C_MAX_CH          1

#define _USE_HW_I2S
#define      HW_I2S_MAX_CH          1

#define _USE_HW_ADC
#define      HW_ADC_MAX_CH          1

#define _USE_HW_PWM
#define      HW_PWM_MAX_CH          1


#define _PIN_GPIO_SDCARD_DETECT     0
#define _PIN_GPIO_SPK_EN            4


#define FLASH_ADDR_BOOT_VER         0x8000400




#endif /* SRC_HW_HW_DEF_H_ */
