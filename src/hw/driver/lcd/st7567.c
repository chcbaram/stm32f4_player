/*
 * st7567.c
 *
 *  Created on: 2020. 12. 30.
 *      Author: baram
 */


#include "lcd/st7567.h"
#include "lcd/st7567_regs.h"
#include "spi.h"
#include "gpio.h"


#ifdef _USE_HW_ST7567

#define _PIN_DEF_BKT    1
#define _PIN_DEF_DC     3
#define _PIN_DEF_CS     2

#define DRIVER_WIDTH    HW_LCD_WIDTH
#define DRIVER_HEIGHT   HW_LCD_HEIGHT


static uint8_t spi_ch = _DEF_SPI1;
static void (*frameCallBack)(void) = NULL;


static bool driverInit(void);
static bool driverReset(void);
static void driverSetWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
static uint16_t driverGetWidth(void);
static uint16_t driverGetHeight(void);
static bool driverSendBuffer(uint8_t *p_data, uint32_t length, uint32_t timeout_ms);
static bool driverSetCallBack(void (*p_func)(void));
static void driverFill(uint16_t color);
static bool driverUpdateDraw(void);
static void driverDrawPixel(uint8_t x, uint8_t y, uint16_t color);


static uint8_t driver_buffer[DRIVER_WIDTH * DRIVER_HEIGHT / 8];




bool st7567Init(lcd_driver_t *p_driver)
{
  bool ret;


  p_driver->init        = driverInit;
  p_driver->reset       = driverReset;
  p_driver->setWindow   = driverSetWindow;
  p_driver->getWidth    = driverGetWidth;
  p_driver->getHeight   = driverGetHeight;
  p_driver->setCallBack = driverSetCallBack;
  p_driver->sendBuffer  = driverSendBuffer;

  ret = driverReset();
  
  return ret;
}

bool driverInit()
{
  bool ret;

  ret = driverReset();
  
  return ret;
}

void writeCommand(uint8_t c)
{
  gpioPinWrite(_PIN_DEF_DC, _DEF_LOW);
  gpioPinWrite(_PIN_DEF_CS, _DEF_LOW);

  spiTransfer8(spi_ch, c);

  gpioPinWrite(_PIN_DEF_CS, _DEF_HIGH);
}

void writeData(uint8_t d)
{
  gpioPinWrite(_PIN_DEF_DC, _DEF_HIGH);
  gpioPinWrite(_PIN_DEF_CS, _DEF_LOW);

  spiTransfer8(spi_ch, d);

  gpioPinWrite(_PIN_DEF_CS, _DEF_HIGH);
}


bool driverReset(void)
{
  bool ret = true;

  spiBegin(spi_ch);
  spiSetDataMode(spi_ch, SPI_MODE0);
  

  gpioPinWrite(_PIN_DEF_BKT, _DEF_LOW);
  gpioPinWrite(_PIN_DEF_DC,  _DEF_HIGH);
  gpioPinWrite(_PIN_DEF_CS,  _DEF_HIGH);
  delay(2);

  writeCommand(0xE2); // Software Reset
  delay(2);
  writeCommand(0xA0 | (0<<0)); // Set scan direction of SEG, 
                               // MX=0, normal direction
  writeCommand(0xC0 | (1<<3)); // Set output direction of COM, 
                               // MY=1, reverse direction
  writeCommand(0xA2 | (0<<0)); // Select bias setting
                               // 0=1/9
  writeCommand(0x28 | (7<<0)); // Control built-in power circuit ON/OFF
  writeCommand(0x24);          // Select regulation resistor ratio 0x25
  writeCommand(0x81);          // Set EV 0x81
  writeCommand(0x19);          // Set EV 0x19
  writeCommand(0x40);
  writeCommand(0xAF);

  driverFill(white);
  driverUpdateDraw();

  return ret;
}

void driverSetWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
}

uint16_t driverGetWidth(void)
{
  return DRIVER_WIDTH;
}

uint16_t driverGetHeight(void)
{
  return DRIVER_HEIGHT;
}

bool driverSendBuffer(uint8_t *p_data, uint32_t length, uint32_t timeout_ms)
{
  uint16_t *p_buf = (uint16_t *)p_data;


  for (int y=0; y<DRIVER_HEIGHT; y++)
  {
    for (int x=0; x<DRIVER_WIDTH; x++)
    {
      driverDrawPixel(x, y, p_buf[y*LCD_WIDTH + x]);
    }
  }


  driverUpdateDraw();

  if (frameCallBack != NULL)
  {
    frameCallBack();
  }


  return true;
}

bool driverSetCallBack(void (*p_func)(void))
{
  frameCallBack = p_func;

  return true;
}

void driverFill(uint16_t color)
{
  uint32_t i;

  for(i = 0; i < sizeof(driver_buffer); i++)
  {
    driver_buffer[i] = (color > 0) ? 0xFF : 0x00;
  }
}

bool driverUpdateDraw(void)
{
  uint8_t i;



  for (i = 0; i < 8; i++)
  {
    writeCommand(0xB0 + i);
    writeCommand(0x00);
    writeCommand(0x10);

    gpioPinWrite(_PIN_DEF_DC, _DEF_HIGH);
    gpioPinWrite(_PIN_DEF_CS, _DEF_LOW);

    spiTransfer(spi_ch, &driver_buffer[DRIVER_WIDTH * i], NULL, DRIVER_WIDTH, 100);

    gpioPinWrite(_PIN_DEF_CS, _DEF_HIGH);	
  }

  return true;
}

void driverDrawPixel(uint8_t x, uint8_t y, uint16_t color)
{
  if (x >= DRIVER_WIDTH || y >= DRIVER_HEIGHT)
  {
    return;
  }


  if (color > 0)
  {
    driver_buffer[x + (y / 8) * DRIVER_WIDTH] |= 1 << (y % 8);
  }
  else
  {
    driver_buffer[x + (y / 8) * DRIVER_WIDTH] &= ~(1 << (y % 8));
  }
}

#endif
