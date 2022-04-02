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
  button_obj_t btn_menu_up;    
  button_obj_t btn_menu_down;    
  button_obj_t btn_menu_enter;    
  uint8_t box_i;
  uint8_t menu_show_offset;
  uint8_t menu_show_cnt;
  uint8_t menu_max_cnt;  
  bool update_screen;  

  sd_state_t sd_state;  
} args_t;

typedef struct 
{
  const char *name;
  void (*func)(void *arg);
} menu_item_t;



LCD_IMAGE_DEF(img_logo);
LCD_IMAGE_DEF(img_logo2);

void cliBoot(cli_args_t *args);
void initArgs(args_t *p_args);
void lcdLoop(args_t *p_args);
void sdLoop(args_t *p_args);


static void runMenuAbout(void *arg);
static void runMenuEmpty(void *arg);

const menu_item_t menu_list[] = 
{
  {"Files", NULL},
  {"Setup", NULL},
  {"Info", NULL},
  {"About", runMenuAbout},
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
  args_t args;


  initArgs(&args);

  while(1)
  {
    if (cliMain() == true)
    {
      args.update_screen = true;
    }
    lcdLoop(&args);   
    sdLoop(&args);
  }
}

void initArgs(args_t *p_args)
{
  p_args->box_i = 0;
  p_args->pre_time = millis();
  p_args->update_screen = true;
  p_args->menu_show_offset = 0;
  p_args->menu_show_cnt = 3;
  p_args->menu_max_cnt = sizeof(menu_list)/sizeof(menu_item_t);

  buttonObjCreate(&p_args->btn_menu_up, 1, 50, 1000, 100);      
  buttonObjCreate(&p_args->btn_menu_down, 2, 50, 1000, 100);      
  buttonObjCreate(&p_args->btn_menu_enter, 3, 50, 1000, 100);      
}

void lcdLoop(args_t *p_args)
{
  if (lcdIsInit() != true)
  {
    return;
  }

  buttonObjUpdate(&p_args->btn_menu_up);
  buttonObjUpdate(&p_args->btn_menu_down);
  buttonObjUpdate(&p_args->btn_menu_enter);

  if (buttonObjGetEvent(&p_args->btn_menu_up) & BUTTON_EVT_CLICKED)
  {
    if (p_args->box_i > 0)
      p_args->box_i = (p_args->box_i - 1);
    else
      p_args->box_i = p_args->menu_max_cnt - 1;

    p_args->update_screen = true;
    buttonObjClearEvent(&p_args->btn_menu_up, BUTTON_EVT_CLICKED);    
  } 

  if (buttonObjGetEvent(&p_args->btn_menu_down) & BUTTON_EVT_CLICKED)
  {
    p_args->box_i = (p_args->box_i + 1) % p_args->menu_max_cnt;

    p_args->update_screen = true;
    buttonObjClearEvent(&p_args->btn_menu_down, BUTTON_EVT_CLICKED);    
  }  

  if (buttonObjGetEvent(&p_args->btn_menu_enter) & BUTTON_EVT_CLICKED)
  {
    if (menu_list[p_args->box_i].func != NULL)
    {
      menu_list[p_args->box_i].func(p_args);
    }  
    else
    {
      runMenuEmpty(p_args);
    }
    buttonObjClearEvent(&p_args->btn_menu_enter, BUTTON_EVT_CLICKED);    
  }  

  if (p_args->box_i < p_args->menu_show_offset)
  {
    p_args->menu_show_offset = p_args->box_i;
  }

  if (p_args->box_i >= (p_args->menu_show_offset + p_args->menu_show_cnt))
  {
    p_args->menu_show_offset = p_args->box_i - (p_args->menu_show_cnt - 1) ;
  }

  if (p_args->update_screen == true)
  {
    p_args->update_screen = false;

    lcdClearBuffer(black);
    lcdDrawRect(0, 0, 128, 64, white);
    lcdPrintf(32, 0, white, "- Menu -");      
    lcdDrawFillRect(0, 16 + 16*(p_args->box_i-p_args->menu_show_offset), 128, 16, white);

    uint8_t i_start;
    uint8_t i_end;

    i_start = p_args->menu_show_offset;
    i_end   = p_args->menu_show_offset + p_args->menu_show_cnt;
    for (int i=i_start; i<i_end; i++)
    {
      if (i == p_args->box_i)
      {
        lcdPrintf(0, 16 + 16*(i-i_start), black, " %d.%s", i+1, menu_list[i]);
      }
      else
      {
        lcdPrintf(0, 16 + 16*(i-i_start), white, " %d.%s", i+1, menu_list[i]);
      }
    }

    lcdRequestDraw();
  }
}

void sdLoop(args_t *p_args)
{
  p_args->sd_state = sdUpdate();
  if (p_args->sd_state == SDCARD_CONNECTED)
  {
    logPrintf("\nSDCARD_CONNECTED\n");
  }
  if (p_args->sd_state == SDCARD_DISCONNECTED)
  {
    logPrintf("\nSDCARD_DISCONNECTED\n");
  }
}

void runMenuAbout(void *arg)
{
  args_t *p_args = (args_t *)arg;
  uint16_t box_w = 128-32;
  uint16_t box_h = 64-16;
  uint16_t box_x;
  uint16_t box_y;

  box_x = (LCD_WIDTH-box_w)/2;
  box_y = (LCD_HEIGHT-box_h)/2;

  lcdDrawFillRect(box_x, box_y, box_w, box_h, white);
  lcdDrawFillRect(box_x+2, box_y+2, box_w-4, box_h-4, black);
  lcdPrintf(16, 16, white, " 만든이");
  lcdPrintf(16, 32, white, " Baram");
  lcdRequestDraw();
  delay(1500);

  p_args->update_screen = true;
}

void runMenuEmpty(void *arg)
{
  args_t *p_args = (args_t *)arg;
  uint16_t box_w = 128-32;
  uint16_t box_h = 32;
  uint16_t box_x;
  uint16_t box_y;

  box_x = (LCD_WIDTH-box_w)/2;
  box_y = (LCD_HEIGHT-box_h)/2;

  lcdDrawFillRect(box_x-2, box_y-2, box_w+4, box_h+4, black);
  lcdDrawFillRect(box_x, box_y, box_w, box_h, white);
  lcdDrawFillRect(box_x+2, box_y+2, box_w-4, box_h-4, black);
  lcdPrintf(16, 24, white, "   Empty");
  lcdRequestDraw();
  delay(1000);

  p_args->update_screen = true;
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
