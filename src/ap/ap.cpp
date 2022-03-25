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
  button_obj_t btn_sw;    
  uint8_t box_i;
  bool update_screen;  
} args_t;



LCD_IMAGE_DEF(img_logo);
LCD_IMAGE_DEF(img_logo2);

void cliBoot(cli_args_t *args);
void lcdMain(args_t *p_args);
void sdMain(args_t *p_args);


const char *menu_list[] = 
{
  "Files",
  "Setup",
  "About"
};



void apInit(void)
{
  lcdSetBackLight(20);

  lcdClearBuffer(black);
  lcdDrawRect(0, 0, 128, 64, white);
  
  lcdDrawFillRect(12+3,12+3, 104, 22, white);
  lcdDrawFillRect(12  ,12  , 104, 22, black);  
  lcdDrawRect    (12  ,12  , 104, 22, white);

  lcdPrintf(16,16, white, "Music Player");    
  lcdSetFont(LCD_FONT_07x10);    
  lcdPrintf(32,42, white, "%s", _DEF_FIRMWATRE_VERSION);    
  lcdRequestDraw();
  delay(1500);
  
  lcdSetFont(LCD_FONT_HAN);

  cliOpen(_DEF_UART1, 57600);   // USB
  cliAdd("boot", cliBoot);
}

void apMain(void)
{
  uint32_t pre_time;
  args_t args;


  

  args.box_i = 0;
  args.pre_time = millis();
  args.update_screen = true;
  buttonObjCreate(&args.btn_sw, 0, 50, 1000, 100);      

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      //ledToggle(_DEF_LED1);
    }

    if (cliMain() == true)
    {
      args.update_screen = true;
    }
    lcdMain(&args);   
    sdMain(&args);
  }
}

void lcdMain(args_t *p_args)
{
  if (lcdIsInit() != true)
  {
    return;
  }

  if (buttonObjUpdate(&p_args->btn_sw) == true)
  {
    if (buttonObjGetEvent(&p_args->btn_sw) & BUTTON_EVT_CLICKED)
    {
      p_args->box_i = (p_args->box_i + 1) % 3;
      p_args->update_screen = true;
    } 
    buttonObjClearEvent(&p_args->btn_sw);
  }
    
  if (p_args->update_screen == true)
  {
    p_args->update_screen = false;

    lcdClearBuffer(black);
    lcdDrawRect(0, 0, 128, 64, white);
    lcdPrintf(32, 0, white, "- Menu -");      
    lcdDrawFillRect(0, 16 + 16*p_args->box_i, 128, 16, white);

    for (int i=0; i<3; i++)
    {
      if (i == p_args->box_i)
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
}

void sdMain(args_t *p_args)
{
  sd_state_t sd_state;


  sd_state = sdUpdate();
  if (sd_state == SDCARD_CONNECTED)
  {
    logPrintf("\nSDCARD_CONNECTED\n");
  }
  if (sd_state == SDCARD_DISCONNECTED)
  {
    logPrintf("\nSDCARD_DISCONNECTED\n");
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
