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

const char *menu_list[] = 
{
  "Files",
  "Setup",
  "About"
};



void apInit(void)
{
  lcdSetBackLight(20);

  cliOpen(_DEF_UART1, 57600);   // USB
  cliAdd("boot", cliBoot);
}

void apMain(void)
{
  uint32_t pre_time;
  args_t args;
  button_obj_t btn_sw;    
  uint8_t box_i = 0;
  bool update_screen = true;

  buttonObjCreate(&btn_sw, 0, 50, 1000, 100);      

  args.mode = 0;
  args.x_time = 0;
  args.pre_time = millis();


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      //ledToggle(_DEF_LED1);
    }

    cliMain();
    if (buttonObjUpdate(&btn_sw) == true)
    {
      if (buttonObjGetEvent(&btn_sw) & BUTTON_EVT_CLICKED)
      {
        box_i = (box_i + 1) % 3;
        update_screen = true;
      } 
      buttonObjClearEvent(&btn_sw);
    }

    
    if (update_screen == true)
    {
      update_screen = false;

      lcdClearBuffer(black);
      lcdDrawRect(0, 0, 128, 64, white);
      lcdPrintf(32, 0, white, "- Menu -");      
      lcdDrawFillRect(0, 16 + 16*box_i, 128, 16, white);

      for (int i=0; i<3; i++)
      {
        if (i == box_i)
        {
          lcdPrintf(0, 16 + 16*i, black, " %d.%s", i+1, menu_list[i]);
        }
        else
        {
          lcdPrintf(0, 16 + 16*i, white, " %d.%s", i+1, menu_list[i]);
        }
      }
      lcdRequestDraw();
    }

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
