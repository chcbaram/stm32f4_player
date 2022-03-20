/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"


typedef struct
{
  uint32_t pre_time;
  uint16_t x_time;
  uint8_t  mode;
} args_t;



LCD_IMAGE_DEF(img_logo);
LCD_IMAGE_DEF(img_logo2);

void cliBoot(cli_args_t *args);
void lcdMain(args_t *p_args);



void apInit(void)
{
  lcdSetBackLight(50);

  cliOpen(_DEF_UART1, 57600);   // USB
  cliAdd("boot", cliBoot);


  //lcdDrawRect(0, 0, 128, 64, white);
  lcdPrintf(32, 0, white, "- Menu -");
  lcdPrintf( 0,16, white, " Files");
  lcdRequestDraw();
}

void apMain(void)
{
  uint32_t pre_time;
  args_t args;

  args.mode = 0;
  args.x_time = 0;
  args.pre_time = millis();


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }

    cliMain();

    //lcdMain(&args);   
  }
}

void lcdMain(args_t *p_args)
{
  if (lcdIsInit() != true)
  {
    return;
  }

  if (millis()-p_args->pre_time >= (1000/5) && lcdDrawAvailable() == true)
  {
    p_args->pre_time = millis();

    lcdClearBuffer(black);

    uint16_t x1 = 0;
    uint16_t x2 = 0;


    if (buttonGetPressed(_DEF_BUTTON1))
    {
      p_args->mode = (p_args->mode + 1)%2;
      delay(200);
    }
    if (p_args->mode == 0)
    {
      p_args->x_time += 2;

      x1 = p_args->x_time;
      x1 %= (LCD_WIDTH-img_logo.header.w);;

      x2 = p_args->x_time;
      x2 %= (LCD_WIDTH-img_logo.header.w);
      x2 = LCD_WIDTH - img_logo.header.w - x2;

      lcdDrawImage(x1, 0, &img_logo);
      lcdDrawImage(x2, 0, &img_logo2);
    }

    if (p_args->mode == 1)
    {
      lcdPrintfResize(0,16, green, 32, "Mode 1");
    }

    lcdRequestDraw();
  }
}



void cliBoot(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    firm_version_t *p_boot_ver = (firm_version_t *)(FLASH_ADDR_BOOT_VER);


    cliPrintf("boot ver   : %s\n", p_boot_ver->version);
    cliPrintf("boot name  : %s\n", p_boot_ver->name);
    cliPrintf("boot param : 0x%X\n", rtcBackupRegRead(0));

    cliPrintf("PCLK2 : %d\n",HAL_RCC_GetPCLK2Freq());

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "jump_boot") == true)
  {
    resetToBoot(0);
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "jump_fw") == true)
  {
    rtcBackupRegWrite(0, 0);
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("boot info\n");
  }
}
