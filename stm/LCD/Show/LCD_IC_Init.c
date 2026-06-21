/**
 * @brief       LCD_IC_Init.c存放各个LCD驱动IC的寄存器初始化部分代码,以简化LCD_Init.c,该.c文件
 *              不直接加入到工程里面,只有lcd.c会用到,所以通过include的形式添加.(不要在
 *              其他文件再包含该.c文件!!否则会报错!) 
 */ 
#include "stm32h7xx.h"                  // Device header

#include "LCD_Init.h"

/**
 * @brief       ST7789 寄存器初始化代码
 * @param       无
 * @retval      无
 */
// void LCD_IC_st7789_reginit(void)
// {
//     LCD_WR_REG(0x11);

//     HAL_Delay(120); 

//     LCD_WR_REG(0x36);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0x3A);
//     LCD_WR_DATA(0X05);

//     LCD_WR_REG(0xB2);
//     LCD_WR_DATA(0x0C);
//     LCD_WR_DATA(0x0C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x33);
//     LCD_WR_DATA(0x33);

//     LCD_WR_REG(0xB7);
//     LCD_WR_DATA(0x35);

//     LCD_WR_REG(0xBB); /* vcom */
//     LCD_WR_DATA(0x32);  /* 30 */

//     LCD_WR_REG(0xC0);
//     LCD_WR_DATA(0x0C);

//     LCD_WR_REG(0xC2);
//     LCD_WR_DATA(0x01);

//     LCD_WR_REG(0xC3); /* vrh */
//     LCD_WR_DATA(0x10);  /* 17 0D */

//     LCD_WR_REG(0xC4); /* vdv */
//     LCD_WR_DATA(0x20);  /* 20 */

//     LCD_WR_REG(0xC6);
//     LCD_WR_DATA(0x0f);

//     LCD_WR_REG(0xD0);
//     LCD_WR_DATA(0xA4); 
//     LCD_WR_DATA(0xA1); 

//     LCD_WR_REG(0xE0); /* Set Gamma  */
//     LCD_WR_DATA(0xd0);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x02);
//     LCD_WR_DATA(0x07);
//     LCD_WR_DATA(0x0a);
//     LCD_WR_DATA(0x28);
//     LCD_WR_DATA(0x32);
//     LCD_WR_DATA(0X44);
//     LCD_WR_DATA(0x42);
//     LCD_WR_DATA(0x06);
//     LCD_WR_DATA(0x0e);
//     LCD_WR_DATA(0x12);
//     LCD_WR_DATA(0x14);
//     LCD_WR_DATA(0x17);

//     LCD_WR_REG(0XE1); /* Set Gamma */
//     LCD_WR_DATA(0xd0);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x02);
//     LCD_WR_DATA(0x07);
//     LCD_WR_DATA(0x0a);
//     LCD_WR_DATA(0x28);
//     LCD_WR_DATA(0x31);
//     LCD_WR_DATA(0x54);
//     LCD_WR_DATA(0x47);
//     LCD_WR_DATA(0x0e);
//     LCD_WR_DATA(0x1c);
//     LCD_WR_DATA(0x17);
//     LCD_WR_DATA(0x1b); 
//     LCD_WR_DATA(0x1e);

//     LCD_WR_REG(0x2A);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xef);

//     LCD_WR_REG(0x2B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x3f);

//     LCD_WR_REG(0x29); /* display on */
// }

// /**
//  * @brief       ILI9341寄存器初始化代码
//  * @param       无
//  * @retval      无
//  */
// void LCD_IC_ili9341_reginit(void)
// {
//     LCD_WR_REG(0xCF);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC1);
//     LCD_WR_DATA(0X30);
//     LCD_WR_REG(0xED);
//     LCD_WR_DATA(0x64);
//     LCD_WR_DATA(0x03);
//     LCD_WR_DATA(0X12);
//     LCD_WR_DATA(0X81);
//     LCD_WR_REG(0xE8);
//     LCD_WR_DATA(0x85);
//     LCD_WR_DATA(0x10);
//     LCD_WR_DATA(0x7A);
//     LCD_WR_REG(0xCB);
//     LCD_WR_DATA(0x39);
//     LCD_WR_DATA(0x2C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x34);
//     LCD_WR_DATA(0x02);
//     LCD_WR_REG(0xF7);
//     LCD_WR_DATA(0x20);
//     LCD_WR_REG(0xEA);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_REG(0xC0); /* Power control */
//     LCD_WR_DATA(0x1B);  /* VRH[5:0] */
//     LCD_WR_REG(0xC1); /* Power control */
//     LCD_WR_DATA(0x01);  /* SAP[2:0];BT[3:0] */
//     LCD_WR_REG(0xC5); /* VCM control */
//     LCD_WR_DATA(0x30);  /* 3F */
//     LCD_WR_DATA(0x30);  /* 3C */
//     LCD_WR_REG(0xC7); /* VCM control2 */
//     LCD_WR_DATA(0XB7);
//     LCD_WR_REG(0x36); /* Memory Access Control */
//     LCD_WR_DATA(0x48);
//     LCD_WR_REG(0x3A);
//     LCD_WR_DATA(0x55);
//     LCD_WR_REG(0xB1);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x1A);
//     LCD_WR_REG(0xB6); /* Display Function Control */
//     LCD_WR_DATA(0x0A);
//     LCD_WR_DATA(0xA2);
//     LCD_WR_REG(0xF2); /* 3Gamma Function Disable */
//     LCD_WR_DATA(0x00);
//     LCD_WR_REG(0x26); /* Gamma curve selected */
//     LCD_WR_DATA(0x01);
//     LCD_WR_REG(0xE0); /* Set Gamma */
//     LCD_WR_DATA(0x0F);
//     LCD_WR_DATA(0x2A);
//     LCD_WR_DATA(0x28);
//     LCD_WR_DATA(0x08);
//     LCD_WR_DATA(0x0E);
//     LCD_WR_DATA(0x08);
//     LCD_WR_DATA(0x54);
//     LCD_WR_DATA(0XA9);
//     LCD_WR_DATA(0x43);
//     LCD_WR_DATA(0x0A);
//     LCD_WR_DATA(0x0F);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_REG(0XE1); /* Set Gamma */
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x15);
//     LCD_WR_DATA(0x17);
//     LCD_WR_DATA(0x07);
//     LCD_WR_DATA(0x11);
//     LCD_WR_DATA(0x06);
//     LCD_WR_DATA(0x2B);
//     LCD_WR_DATA(0x56);
//     LCD_WR_DATA(0x3C);
//     LCD_WR_DATA(0x05);
//     LCD_WR_DATA(0x10);
//     LCD_WR_DATA(0x0F);
//     LCD_WR_DATA(0x3F);
//     LCD_WR_DATA(0x3F);
//     LCD_WR_DATA(0x0F);
//     LCD_WR_REG(0x2B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x3f);
//     LCD_WR_REG(0x2A);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xef);
//     LCD_WR_REG(0x11); /* Exit Sleep */
//     HAL_Delay(120);
//     LCD_WR_REG(0x29); /* display on */
// }
 

// /**
//  * @brief       NT35310寄存器初始化代码 
//  * @param       无
//  * @retval      无
//  */
// void LCD_IC_nt35310_reginit(void)
// {
//     LCD_WR_REG(0xED);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0xFE);

//     LCD_WR_REG(0xEE);
//     LCD_WR_DATA(0xDE);
//     LCD_WR_DATA(0x21);

//     LCD_WR_REG(0xF1);
//     LCD_WR_DATA(0x01);
//     LCD_WR_REG(0xDF);
//     LCD_WR_DATA(0x10);

//     /* VCOMvoltage */
//     LCD_WR_REG(0xC4);
//     LCD_WR_DATA(0x8F);  /* 5f */

//     LCD_WR_REG(0xC6);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xE2);
//     LCD_WR_DATA(0xE2);
//     LCD_WR_DATA(0xE2);
//     LCD_WR_REG(0xBF);
//     LCD_WR_DATA(0xAA);

//     LCD_WR_REG(0xB0);
//     LCD_WR_DATA(0x0D);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x0D);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x11);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x19);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x21);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x2D);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x3D);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x5D);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x5D);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xB1);
//     LCD_WR_DATA(0x80);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x8B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x96);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xB2);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x02);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x03);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xB3);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xB4);
//     LCD_WR_DATA(0x8B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x96);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA1);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xB5);
//     LCD_WR_DATA(0x02);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x03);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x04);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xB6);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xB7);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x3F);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x5E);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x64);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x8C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xAC);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xDC);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x70);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x90);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xEB);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xDC);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xB8);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xBA);
//     LCD_WR_DATA(0x24);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC1);
//     LCD_WR_DATA(0x20);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x54);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xFF);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC2);
//     LCD_WR_DATA(0x0A);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x04);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC3);
//     LCD_WR_DATA(0x3C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x3A);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x39);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x37);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x3C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x36);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x32);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x2F);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x2C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x29);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x26);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x24);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x24);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x23);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x3C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x36);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x32);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x2F);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x2C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x29);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x26);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x24);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x24);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x23);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC4);
//     LCD_WR_DATA(0x62);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x05);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x84);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xF0);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x18);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA4);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x18);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x50);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x0C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x17);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x95);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xF3);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xE6);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC5);
//     LCD_WR_DATA(0x32);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x44);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x65);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x76);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x88);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC6);
//     LCD_WR_DATA(0x20);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x17);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC7);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC8);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xC9);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xE0);
//     LCD_WR_DATA(0x16);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x1C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x21);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x36);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x46);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x52);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x64);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x7A);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x8B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x99);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA8);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xB9);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC4);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xCA);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD2);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD9);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xE0);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xF3);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xE1);
//     LCD_WR_DATA(0x16);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x1C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x22);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x36);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x45);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x52);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x64);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x7A);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x8B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x99);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA8);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xB9);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC4);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xCA);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD2);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD8);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xE0);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xF3);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xE2);
//     LCD_WR_DATA(0x05);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x0B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x1B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x34);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x44);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x4F);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x61);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x79);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x88);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x97);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA6);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xB7);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC2);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC7);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD1);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD6);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xDD);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xF3);
//     LCD_WR_DATA(0x00);
//     LCD_WR_REG(0xE3);
//     LCD_WR_DATA(0x05);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x1C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x33);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x44);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x50);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x62);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x78);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x88);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x97);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA6);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xB7);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC2);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC7);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD1);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD5);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xDD);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xF3);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xE4);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x02);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x2A);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x3C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x4B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x5D);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x74);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x84);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x93);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA2);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xB3);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xBE);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC4);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xCD);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD3);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xDD);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xF3);
//     LCD_WR_DATA(0x00);
//     LCD_WR_REG(0xE5);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x02);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x29);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x3C);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x4B);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x5D);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x74);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x84);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x93);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xA2);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xB3);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xBE);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xC4);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xCD);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xD3);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xDC);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xF3);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xE6);
//     LCD_WR_DATA(0x11);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x34);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x56);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x76);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x77);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x66);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x88);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x99);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xBB);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x99);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x66);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x55);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x55);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x45);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x43);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x44);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xE7);
//     LCD_WR_DATA(0x32);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x55);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x76);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x66);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x67);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x67);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x87);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x99);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xBB);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x99);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x77);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x44);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x56);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x23);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x33);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x45);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xE8);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x99);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x87);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x88);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x77);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x66);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x88);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xAA);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xBB);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x99);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x66);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x55);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x55);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x44);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x44);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x55);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xE9);
//     LCD_WR_DATA(0xAA);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0x00);
//     LCD_WR_DATA(0xAA);

//     LCD_WR_REG(0xCF);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xF0);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x50);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xF3);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xF9);
//     LCD_WR_DATA(0x06);
//     LCD_WR_DATA(0x10);
//     LCD_WR_DATA(0x29);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0x3A);
//     LCD_WR_DATA(0x55);  /* 66 */

//     LCD_WR_REG(0x11);
//     HAL_Delay(100);
//     LCD_WR_REG(0x29);
//     LCD_WR_REG(0x35);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0x51);
//     LCD_WR_DATA(0xFF);
//     LCD_WR_REG(0x53);
//     LCD_WR_DATA(0x2C);
//     LCD_WR_REG(0x55);
//     LCD_WR_DATA(0x82);
//     LCD_WR_REG(0x2c);
// }

// /**
//  * @brief       ST7796寄存器初始化代码 
//  * @param       无
//  * @retval      无
//  */
// void LCD_IC_st7796_reginit(void)
// {
//     LCD_WR_REG(0x11);

//     HAL_Delay(120); 

//     LCD_WR_REG(0x36); /* Memory Data Access Control MY,MX~~ */
//     LCD_WR_DATA(0x48);
    
//     LCD_WR_REG(0x3A);
//     LCD_WR_DATA(0x55);
    
//     LCD_WR_REG(0xF0);
//     LCD_WR_DATA(0xC3);
    
//     LCD_WR_REG(0xF0);
//     LCD_WR_DATA(0x96);

//     LCD_WR_REG(0xB4);
//     LCD_WR_DATA(0x01);
    
//     LCD_WR_REG(0xB6); /* Display Function Control */
//     LCD_WR_DATA(0x0A);
//     LCD_WR_DATA(0xA2);

//     LCD_WR_REG(0xB7);
//     LCD_WR_DATA(0xC6);

//     LCD_WR_REG(0xB9);
//     LCD_WR_DATA(0x02);
//     LCD_WR_DATA(0xE0);

//     LCD_WR_REG(0xC0);
//     LCD_WR_DATA(0x80);
//     LCD_WR_DATA(0x16);

//     LCD_WR_REG(0xC1);
//     LCD_WR_DATA(0x19);

//     LCD_WR_REG(0xC2);
//     LCD_WR_DATA(0xA7);

//     LCD_WR_REG(0xC5);
//     LCD_WR_DATA(0x16);   

//     LCD_WR_REG(0xE8);
//     LCD_WR_DATA(0x40);
//     LCD_WR_DATA(0x8A);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x29);
//     LCD_WR_DATA(0x19);
//     LCD_WR_DATA(0xA5);
//     LCD_WR_DATA(0x33);

//     LCD_WR_REG(0xE0);
//     LCD_WR_DATA(0xF0);
//     LCD_WR_DATA(0x07);
//     LCD_WR_DATA(0x0D);
//     LCD_WR_DATA(0x04);
//     LCD_WR_DATA(0x05);
//     LCD_WR_DATA(0x14);
//     LCD_WR_DATA(0x36);
//     LCD_WR_DATA(0x54);
//     LCD_WR_DATA(0x4C);
//     LCD_WR_DATA(0x38);
//     LCD_WR_DATA(0x13);
//     LCD_WR_DATA(0x14);
//     LCD_WR_DATA(0x2E);
//     LCD_WR_DATA(0x34);

//     LCD_WR_REG(0xE1);
//     LCD_WR_DATA(0xF0);
//     LCD_WR_DATA(0x10);
//     LCD_WR_DATA(0x14);
//     LCD_WR_DATA(0x0E);
//     LCD_WR_DATA(0x0C);
//     LCD_WR_DATA(0x08);
//     LCD_WR_DATA(0x35);
//     LCD_WR_DATA(0x44);
//     LCD_WR_DATA(0x4C);
//     LCD_WR_DATA(0x26);
//     LCD_WR_DATA(0x10);
//     LCD_WR_DATA(0x12);
//     LCD_WR_DATA(0x2C);
//     LCD_WR_DATA(0x32);

//     LCD_WR_REG(0xF0);
//     LCD_WR_DATA(0x3C);

//     LCD_WR_REG(0xF0);
//     LCD_WR_DATA(0x69);

//     HAL_Delay(120);

//     LCD_WR_REG(0x21);
//     LCD_WR_REG(0x29);
// }

/**
 * @brief       NT35510寄存器初始化代码 
 * @param       无
 * @retval      无
 */
void LCD_IC_nt35510_reginit(void)
{
    LCD_WriteReg(0xF000, 0x55);
    LCD_WriteReg(0xF001, 0xAA);
    LCD_WriteReg(0xF002, 0x52);
    LCD_WriteReg(0xF003, 0x08);
    LCD_WriteReg(0xF004, 0x01);
    /* AVDD ratio */
    LCD_WriteReg(0xB600, 0x34); /* 34 */
    LCD_WriteReg(0xB601, 0x34); /* 34 */
    LCD_WriteReg(0xB602, 0x34); /* 34 */
    /* AVDD 5.2V */
    LCD_WriteReg(0xB000, 0x0D); /* 0C,减小，AVDD电压增大 */
    LCD_WriteReg(0xB001, 0x0D); /* 0C */
    LCD_WriteReg(0xB002, 0x0D); /* 0C */
     
    /* AVEE ratio */
    LCD_WriteReg(0xB700, 0x24); /* 24 */
    LCD_WriteReg(0xB701, 0x24); /* 24 */
    LCD_WriteReg(0xB702, 0x24); /* 24 */
    /* AVEE -5.2V */
    LCD_WriteReg(0xB100, 0x0D); /* 0C减小，AVEE电压增大 */
    LCD_WriteReg(0xB101, 0x0D); /* 0C */
    LCD_WriteReg(0xB102, 0x0D); /* 0C */

    /* VCL */
    LCD_WriteReg(0xB800, 0x24);
    LCD_WriteReg(0xB801, 0x24);
    LCD_WriteReg(0xB802, 0x24);

    LCD_WriteReg(0xB200, 0x00);

    /***************VGH ratio***************/
    /* VGH ratio */
    LCD_WriteReg(0xB900, 0x24); /* 14 */
    LCD_WriteReg(0xB901, 0x24); /* 14 */
    LCD_WriteReg(0xB902, 0x24); /* 14 */
    /* VGH 10.2V */
    LCD_WriteReg(0xB300, 0x05); /* 08 */
    LCD_WriteReg(0xB301, 0x05); /* 08 */
    LCD_WriteReg(0xB302, 0x05); /* 08 */

    /***************VGLX ratio***************/
    /* VGLX ratio */
    LCD_WriteReg(0xBA00, 0x34);
    LCD_WriteReg(0xBA01, 0x34);
    LCD_WriteReg(0xBA02, 0x34);
    /* VGL_REG -12.6V */
    LCD_WriteReg(0xB500, 0x0B);
    LCD_WriteReg(0xB501, 0x0B);
    LCD_WriteReg(0xB502, 0x0B);

    /***************VGMP/VGSP 4.5V***********/
    LCD_WriteReg(0xBC00, 0x00); /* 00 */
    LCD_WriteReg(0xBC01, 0xA3); /* 80 */
    LCD_WriteReg(0xBC02, 0x00); /* 00 */

    /* GMN/VGSN -4.5V */
    LCD_WriteReg(0xBD00, 0x00); /* 00 */
    LCD_WriteReg(0xBD01, 0xA3); /* 80 */
    LCD_WriteReg(0xBD02, 0x00); /* 00 */

    /* VCOM  -1.25V */
    LCD_WriteReg(0xBE00, 0x00);
    LCD_WriteReg(0xBE01, 0x37); /* 64~2F */


    /***************Gamma Setting***************/
    LCD_WriteReg(0xD100, 0x00);
    LCD_WriteReg(0xD101, 0x37);
    LCD_WriteReg(0xD102, 0x00);
    LCD_WriteReg(0xD103, 0x53);
    LCD_WriteReg(0xD104, 0x00);
    LCD_WriteReg(0xD105, 0x79);
    LCD_WriteReg(0xD106, 0x00);
    LCD_WriteReg(0xD107, 0x97);
    LCD_WriteReg(0xD108, 0x00);
    LCD_WriteReg(0xD109, 0xB1);
    LCD_WriteReg(0xD10A, 0x00);
    LCD_WriteReg(0xD10B, 0xD5);
    LCD_WriteReg(0xD10C, 0x00);
    LCD_WriteReg(0xD10D, 0xF4);
    LCD_WriteReg(0xD10E, 0x01);
    LCD_WriteReg(0xD10F, 0x23);
    LCD_WriteReg(0xD110, 0x01);
    LCD_WriteReg(0xD111, 0x49);
    LCD_WriteReg(0xD112, 0x01);
    LCD_WriteReg(0xD113, 0x87);
    LCD_WriteReg(0xD114, 0x01);
    LCD_WriteReg(0xD115, 0xB6);
    LCD_WriteReg(0xD116, 0x02);
    LCD_WriteReg(0xD117, 0x00);
    LCD_WriteReg(0xD118, 0x02);
    LCD_WriteReg(0xD119, 0x3B);
    LCD_WriteReg(0xD11A, 0x02);
    LCD_WriteReg(0xD11B, 0x3D);
    LCD_WriteReg(0xD11C, 0x02);
    LCD_WriteReg(0xD11D, 0x75);
    LCD_WriteReg(0xD11E, 0x02);
    LCD_WriteReg(0xD11F, 0xB1);
    LCD_WriteReg(0xD120, 0x02);
    LCD_WriteReg(0xD121, 0xD5);
    LCD_WriteReg(0xD122, 0x03);
    LCD_WriteReg(0xD123, 0x09);
    LCD_WriteReg(0xD124, 0x03);
    LCD_WriteReg(0xD125, 0x28);
    LCD_WriteReg(0xD126, 0x03);
    LCD_WriteReg(0xD127, 0x52);
    LCD_WriteReg(0xD128, 0x03);
    LCD_WriteReg(0xD129, 0x6B);
    LCD_WriteReg(0xD12A, 0x03);
    LCD_WriteReg(0xD12B, 0x8D);
    LCD_WriteReg(0xD12C, 0x03);
    LCD_WriteReg(0xD12D, 0xA2);
    LCD_WriteReg(0xD12E, 0x03);
    LCD_WriteReg(0xD12F, 0xBB);
    LCD_WriteReg(0xD130, 0x03);
    LCD_WriteReg(0xD131, 0xC1);
    LCD_WriteReg(0xD132, 0x03);
    LCD_WriteReg(0xD133, 0xC1);
                     
    LCD_WriteReg(0xD200, 0x00);
    LCD_WriteReg(0xD201, 0x37);
    LCD_WriteReg(0xD202, 0x00);
    LCD_WriteReg(0xD203, 0x53);
    LCD_WriteReg(0xD204, 0x00);
    LCD_WriteReg(0xD205, 0x79);
    LCD_WriteReg(0xD206, 0x00);
    LCD_WriteReg(0xD207, 0x97);
    LCD_WriteReg(0xD208, 0x00);
    LCD_WriteReg(0xD209, 0xB1);
    LCD_WriteReg(0xD20A, 0x00);
    LCD_WriteReg(0xD20B, 0xD5);
    LCD_WriteReg(0xD20C, 0x00);
    LCD_WriteReg(0xD20D, 0xF4);
    LCD_WriteReg(0xD20E, 0x01);
    LCD_WriteReg(0xD20F, 0x23);
    LCD_WriteReg(0xD210, 0x01);
    LCD_WriteReg(0xD211, 0x49);
    LCD_WriteReg(0xD212, 0x01);
    LCD_WriteReg(0xD213, 0x87);
    LCD_WriteReg(0xD214, 0x01);
    LCD_WriteReg(0xD215, 0xB6);
    LCD_WriteReg(0xD216, 0x02);
    LCD_WriteReg(0xD217, 0x00);
    LCD_WriteReg(0xD218, 0x02);
    LCD_WriteReg(0xD219, 0x3B);
    LCD_WriteReg(0xD21A, 0x02);
    LCD_WriteReg(0xD21B, 0x3D);
    LCD_WriteReg(0xD21C, 0x02);
    LCD_WriteReg(0xD21D, 0x75);
    LCD_WriteReg(0xD21E, 0x02);
    LCD_WriteReg(0xD21F, 0xB1);
    LCD_WriteReg(0xD220, 0x02);
    LCD_WriteReg(0xD221, 0xD5);
    LCD_WriteReg(0xD222, 0x03);
    LCD_WriteReg(0xD223, 0x09);
    LCD_WriteReg(0xD224, 0x03);
    LCD_WriteReg(0xD225, 0x28);
    LCD_WriteReg(0xD226, 0x03);
    LCD_WriteReg(0xD227, 0x52);
    LCD_WriteReg(0xD228, 0x03);
    LCD_WriteReg(0xD229, 0x6B);
    LCD_WriteReg(0xD22A, 0x03);
    LCD_WriteReg(0xD22B, 0x8D);
    LCD_WriteReg(0xD22C, 0x03);
    LCD_WriteReg(0xD22D, 0xA2);
    LCD_WriteReg(0xD22E, 0x03);
    LCD_WriteReg(0xD22F, 0xBB);
    LCD_WriteReg(0xD230, 0x03);
    LCD_WriteReg(0xD231, 0xC1);
    LCD_WriteReg(0xD232, 0x03);
    LCD_WriteReg(0xD233, 0xC1);

    LCD_WriteReg(0xD300, 0x00);
    LCD_WriteReg(0xD301, 0x37);
    LCD_WriteReg(0xD302, 0x00);
    LCD_WriteReg(0xD303, 0x53);
    LCD_WriteReg(0xD304, 0x00);
    LCD_WriteReg(0xD305, 0x79);
    LCD_WriteReg(0xD306, 0x00);
    LCD_WriteReg(0xD307, 0x97);
    LCD_WriteReg(0xD308, 0x00);
    LCD_WriteReg(0xD309, 0xB1);
    LCD_WriteReg(0xD30A, 0x00);
    LCD_WriteReg(0xD30B, 0xD5);
    LCD_WriteReg(0xD30C, 0x00);
    LCD_WriteReg(0xD30D, 0xF4);
    LCD_WriteReg(0xD30E, 0x01);
    LCD_WriteReg(0xD30F, 0x23);
    LCD_WriteReg(0xD310, 0x01);
    LCD_WriteReg(0xD311, 0x49);
    LCD_WriteReg(0xD312, 0x01);
    LCD_WriteReg(0xD313, 0x87);
    LCD_WriteReg(0xD314, 0x01); 
    LCD_WriteReg(0xD315, 0xB6);
    LCD_WriteReg(0xD316, 0x02);
    LCD_WriteReg(0xD317, 0x00);
    LCD_WriteReg(0xD318, 0x02);
    LCD_WriteReg(0xD319, 0x3b);
    LCD_WriteReg(0xD31A, 0x02);
    LCD_WriteReg(0xD31B, 0x3d);
    LCD_WriteReg(0xD31C, 0x02);
    LCD_WriteReg(0xD31D, 0x75);
    LCD_WriteReg(0xD31E, 0x02);
    LCD_WriteReg(0xD31F, 0xB1);
    LCD_WriteReg(0xD320, 0x02);
    LCD_WriteReg(0xD321, 0xD5);
    LCD_WriteReg(0xD322, 0x03);
    LCD_WriteReg(0xD323, 0x09);
    LCD_WriteReg(0xD324, 0x03);
    LCD_WriteReg(0xD325, 0x28);
    LCD_WriteReg(0xD326, 0x03);
    LCD_WriteReg(0xD327, 0x52);
    LCD_WriteReg(0xD328, 0x03);
    LCD_WriteReg(0xD329, 0x6B);
    LCD_WriteReg(0xD32A, 0x03);
    LCD_WriteReg(0xD32B, 0x8D);
    LCD_WriteReg(0xD32C, 0x03);
    LCD_WriteReg(0xD32D, 0xA2);
    LCD_WriteReg(0xD32E, 0x03);
    LCD_WriteReg(0xD32F, 0xBB);
    LCD_WriteReg(0xD330, 0x03);
    LCD_WriteReg(0xD331, 0xC1);
    LCD_WriteReg(0xD332, 0x03);
    LCD_WriteReg(0xD333, 0xC1);

    LCD_WriteReg(0xD400, 0x00);
    LCD_WriteReg(0xD401, 0x37);
    LCD_WriteReg(0xD402, 0x00);
    LCD_WriteReg(0xD403, 0x53);
    LCD_WriteReg(0xD404, 0x00);
    LCD_WriteReg(0xD405, 0x79);
    LCD_WriteReg(0xD406, 0x00);
    LCD_WriteReg(0xD407, 0x97);
    LCD_WriteReg(0xD408, 0x00);
    LCD_WriteReg(0xD409, 0xB1);
    LCD_WriteReg(0xD40A, 0x00);
    LCD_WriteReg(0xD40B, 0xD5);
    LCD_WriteReg(0xD40C, 0x00);
    LCD_WriteReg(0xD40D, 0xF4);
    LCD_WriteReg(0xD40E, 0x01);
    LCD_WriteReg(0xD40F, 0x23);
    LCD_WriteReg(0xD410, 0x01);
    LCD_WriteReg(0xD411, 0x49);
    LCD_WriteReg(0xD412, 0x01);
    LCD_WriteReg(0xD413, 0x87);
    LCD_WriteReg(0xD414, 0x01);
    LCD_WriteReg(0xD415, 0xB6);
    LCD_WriteReg(0xD416, 0x02);
    LCD_WriteReg(0xD417, 0x00);
    LCD_WriteReg(0xD418, 0x02);
    LCD_WriteReg(0xD419, 0x3B);
    LCD_WriteReg(0xD41A, 0x02);
    LCD_WriteReg(0xD41B, 0x3D);
    LCD_WriteReg(0xD41C, 0x02);
    LCD_WriteReg(0xD41D, 0x75);
    LCD_WriteReg(0xD41E, 0x02);
    LCD_WriteReg(0xD41F, 0xB1);
    LCD_WriteReg(0xD420, 0x02);
    LCD_WriteReg(0xD421, 0xD5);
    LCD_WriteReg(0xD422, 0x03);
    LCD_WriteReg(0xD423, 0x09);
    LCD_WriteReg(0xD424, 0x03);
    LCD_WriteReg(0xD425, 0x28);
    LCD_WriteReg(0xD426, 0x03);
    LCD_WriteReg(0xD427, 0x52);
    LCD_WriteReg(0xD428, 0x03);
    LCD_WriteReg(0xD429, 0x6B);
    LCD_WriteReg(0xD42A, 0x03);
    LCD_WriteReg(0xD42B, 0x8D);
    LCD_WriteReg(0xD42C, 0x03);
    LCD_WriteReg(0xD42D, 0xA2);
    LCD_WriteReg(0xD42E, 0x03);
    LCD_WriteReg(0xD42F, 0xBB);
    LCD_WriteReg(0xD430, 0x03);
    LCD_WriteReg(0xD431, 0xC1);
    LCD_WriteReg(0xD432, 0x03);
    LCD_WriteReg(0xD433, 0xC1);

    LCD_WriteReg(0xD500, 0x00);
    LCD_WriteReg(0xD501, 0x37);
    LCD_WriteReg(0xD502, 0x00);
    LCD_WriteReg(0xD503, 0x53);
    LCD_WriteReg(0xD504, 0x00);
    LCD_WriteReg(0xD505, 0x79);
    LCD_WriteReg(0xD506, 0x00);
    LCD_WriteReg(0xD507, 0x97);
    LCD_WriteReg(0xD508, 0x00);
    LCD_WriteReg(0xD509, 0xB1);
    LCD_WriteReg(0xD50A, 0x00);
    LCD_WriteReg(0xD50B, 0xD5);
    LCD_WriteReg(0xD50C, 0x00);
    LCD_WriteReg(0xD50D, 0xF4);
    LCD_WriteReg(0xD50E, 0x01);
    LCD_WriteReg(0xD50F, 0x23);
    LCD_WriteReg(0xD510, 0x01);
    LCD_WriteReg(0xD511, 0x49);
    LCD_WriteReg(0xD512, 0x01);
    LCD_WriteReg(0xD513, 0x87);
    LCD_WriteReg(0xD514, 0x01);
    LCD_WriteReg(0xD515, 0xB6);
    LCD_WriteReg(0xD516, 0x02);
    LCD_WriteReg(0xD517, 0x00);
    LCD_WriteReg(0xD518, 0x02);
    LCD_WriteReg(0xD519, 0x3B);
    LCD_WriteReg(0xD51A, 0x02);
    LCD_WriteReg(0xD51B, 0x3D);
    LCD_WriteReg(0xD51C, 0x02);
    LCD_WriteReg(0xD51D, 0x75);
    LCD_WriteReg(0xD51E, 0x02);
    LCD_WriteReg(0xD51F, 0xB1);
    LCD_WriteReg(0xD520, 0x02);
    LCD_WriteReg(0xD521, 0xD5);
    LCD_WriteReg(0xD522, 0x03);
    LCD_WriteReg(0xD523, 0x09);
    LCD_WriteReg(0xD524, 0x03);
    LCD_WriteReg(0xD525, 0x28);
    LCD_WriteReg(0xD526, 0x03);
    LCD_WriteReg(0xD527, 0x52);
    LCD_WriteReg(0xD528, 0x03);
    LCD_WriteReg(0xD529, 0x6B);
    LCD_WriteReg(0xD52A, 0x03);
    LCD_WriteReg(0xD52B, 0x8D);
    LCD_WriteReg(0xD52C, 0x03);
    LCD_WriteReg(0xD52D, 0xA2);
    LCD_WriteReg(0xD52E, 0x03);
    LCD_WriteReg(0xD52F, 0xBB);
    LCD_WriteReg(0xD530, 0x03);
    LCD_WriteReg(0xD531, 0xC1);
    LCD_WriteReg(0xD532, 0x03);
    LCD_WriteReg(0xD533, 0xC1);

    LCD_WriteReg(0xD600, 0x00);
    LCD_WriteReg(0xD601, 0x37);
    LCD_WriteReg(0xD602, 0x00);
    LCD_WriteReg(0xD603, 0x53);
    LCD_WriteReg(0xD604, 0x00);
    LCD_WriteReg(0xD605, 0x79);
    LCD_WriteReg(0xD606, 0x00);
    LCD_WriteReg(0xD607, 0x97);
    LCD_WriteReg(0xD608, 0x00);
    LCD_WriteReg(0xD609, 0xB1);
    LCD_WriteReg(0xD60A, 0x00);
    LCD_WriteReg(0xD60B, 0xD5);
    LCD_WriteReg(0xD60C, 0x00);
    LCD_WriteReg(0xD60D, 0xF4);
    LCD_WriteReg(0xD60E, 0x01);
    LCD_WriteReg(0xD60F, 0x23);
    LCD_WriteReg(0xD610, 0x01);
    LCD_WriteReg(0xD611, 0x49);
    LCD_WriteReg(0xD612, 0x01);
    LCD_WriteReg(0xD613, 0x87);
    LCD_WriteReg(0xD614, 0x01);
    LCD_WriteReg(0xD615, 0xB6);
    LCD_WriteReg(0xD616, 0x02);
    LCD_WriteReg(0xD617, 0x00);
    LCD_WriteReg(0xD618, 0x02);
    LCD_WriteReg(0xD619, 0x3B);
    LCD_WriteReg(0xD61A, 0x02);
    LCD_WriteReg(0xD61B, 0x3D);
    LCD_WriteReg(0xD61C, 0x02);
    LCD_WriteReg(0xD61D, 0x75);
    LCD_WriteReg(0xD61E, 0x02);
    LCD_WriteReg(0xD61F, 0xB1);
    LCD_WriteReg(0xD620, 0x02);
    LCD_WriteReg(0xD621, 0xD5);
    LCD_WriteReg(0xD622, 0x03);
    LCD_WriteReg(0xD623, 0x09);
    LCD_WriteReg(0xD624, 0x03);
    LCD_WriteReg(0xD625, 0x28);
    LCD_WriteReg(0xD626, 0x03);
    LCD_WriteReg(0xD627, 0x52);
    LCD_WriteReg(0xD628, 0x03);
    LCD_WriteReg(0xD629, 0x6B);
    LCD_WriteReg(0xD62A, 0x03);
    LCD_WriteReg(0xD62B, 0x8D);
    LCD_WriteReg(0xD62C, 0x03);
    LCD_WriteReg(0xD62D, 0xA2);
    LCD_WriteReg(0xD62E, 0x03);
    LCD_WriteReg(0xD62F, 0xBB);
    LCD_WriteReg(0xD630, 0x03);
    LCD_WriteReg(0xD631, 0xC1);
    LCD_WriteReg(0xD632, 0x03);
    LCD_WriteReg(0xD633, 0xC1);

    /***************LV2 Page 0 enable***************/
    LCD_WriteReg(0xF000, 0x55);
    LCD_WriteReg(0xF001, 0xAA);
    LCD_WriteReg(0xF002, 0x52);
    LCD_WriteReg(0xF003, 0x08);
    LCD_WriteReg(0xF004, 0x00);

    /***************480x800***************/
    //LCD_WriteReg(0xB500, 0x50);
    //RGB Internal mode
    //REGW 0xB30001
    //RGB I/F Setting

    LCD_WriteReg(0xB000, 0x00); 
    LCD_WriteReg(0xB001, 0x05); 
    LCD_WriteReg(0xB002, 0x02);
    LCD_WriteReg(0xB003, 0x05); 
    LCD_WriteReg(0xB004, 0x02);

    /*## SDT: */
    LCD_WriteReg(0xB600, 0x08);
    LCD_WriteReg(0xB500, 0x50); /* 0x6b 480x854       0x50 480x800 */

    /***************Display control***************/
    //LCD_WriteReg(0xB100, 0xCC);
    //LCD_WriteReg(0xB101, 0x00);

    /***************Source hold time***************/
    LCD_WriteReg(0xB600, 0x05);

    /***************Gate EQ control***************/
    LCD_WriteReg(0xB700, 0x00);
    LCD_WriteReg(0xB701, 0x00);


    /***************Source EQ control (Mode 2)***************/
    LCD_WriteReg(0xB800, 0x01);
    LCD_WriteReg(0xB801, 0x05);
    LCD_WriteReg(0xB802, 0x05);
    LCD_WriteReg(0xB803, 0x05);
                    
    /***************Inversion mode  (2-dot)***************/
    LCD_WriteReg(0xBC00, 0x02);//02,2-dot inversion
    LCD_WriteReg(0xBC01, 0x00);
    LCD_WriteReg(0xBC02, 0x00);
                     

    /*************** BOE SETTING(DEFAULT) ***************/
    LCD_WriteReg(0xCC00, 0x03);
    LCD_WriteReg(0xCC01, 0x00);
    LCD_WriteReg(0xCC02, 0x00);

    /***************Frame rate***************/
    LCD_WriteReg(0xBD00, 0x01);
    LCD_WriteReg(0xBD01, 0x84);
    LCD_WriteReg(0xBD02, 0x07);  /* 0X1C   0x07 */
    LCD_WriteReg(0xBD03, 0x31);
    LCD_WriteReg(0xBD04, 0x00);


    LCD_WriteReg(0xBA00, 0x01);

    LCD_WriteReg(0xFF00, 0xAA);
    LCD_WriteReg(0xFF01, 0x55);
    LCD_WriteReg(0xFF02, 0x25);
    LCD_WriteReg(0xFF03, 0x01);

    /*************** Timing control 4H w/ 4-Delayms ***************/
    LCD_WriteReg(0x3600, 0x40);
    LCD_WriteReg(0x3500, 0x00);

    LCD_WriteReg(0x3A00, 0x05);

    LCD_WR_REG(0x1100);
    HAL_Delay(120);
    LCD_WR_REG(0x2900);
    HAL_Delay(10);
    LCD_WR_REG(0x2C00);
}

/**
 * @brief       ILI9806寄存器初始化代码 
 * @param       无
 * @retval      无
 */
// void LCD_IC_ili9806_reginit(void)
// {
//     LCD_WR_REG(0xFF); /* EXTC Command Set enable register */
//     LCD_WR_DATA(0xFF);
//     LCD_WR_DATA(0x98);
//     LCD_WR_DATA(0x06);

//     LCD_WR_REG(0xBC); /* GIP 1 */
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x0F);
//     LCD_WR_DATA(0x61);
//     LCD_WR_DATA(0xFF);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x0B);
//     LCD_WR_DATA(0x10);
//     LCD_WR_DATA(0x37);
//     LCD_WR_DATA(0x63);
//     LCD_WR_DATA(0xFF);
//     LCD_WR_DATA(0xFF);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0xFF);
//     LCD_WR_DATA(0x52);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x40);

//     LCD_WR_REG(0xBD); /* GIP 2 */
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x23);
//     LCD_WR_DATA(0x45);
//     LCD_WR_DATA(0x67);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0x23);
//     LCD_WR_DATA(0x45);
//     LCD_WR_DATA(0x67);

//     LCD_WR_REG(0xBE); /* GIP 3 */
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0xAB);
//     LCD_WR_DATA(0x60);
//     LCD_WR_DATA(0x22);
//     LCD_WR_DATA(0x22);
//     LCD_WR_DATA(0x22);
//     LCD_WR_DATA(0x22);
//     LCD_WR_DATA(0x22);

//     LCD_WR_REG(0xC7); /* VCOM Control */
//     LCD_WR_DATA(0x36);

//     LCD_WR_REG(0xED); /* EN_volt_reg VGMP / VGMN /VGSP / VGSN voltage to output */
//     LCD_WR_DATA(0x7F);
//     LCD_WR_DATA(0x0F);

//     LCD_WR_REG(0XC0); /* Power Control 1 Setting AVDD / AVEE / VGH / VGL */
//     LCD_WR_DATA(0x0F);
//     LCD_WR_DATA(0x0B);
//     LCD_WR_DATA(0x0A);  /* VGH 15V,VGLO-10V */

//     LCD_WR_REG(0XFC); /* AVDD / AVEE generated by internal pumping. */
//     LCD_WR_DATA(0x08);

//     LCD_WR_REG(0XDF); 
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x20);

//     LCD_WR_REG(0XF3); /* DVDD Voltage Setting */
//     LCD_WR_DATA(0x74);

//     LCD_WR_REG(0xB4); /* Inversion Type */
//     LCD_WR_DATA(0x00);  /* 02 */
//     LCD_WR_DATA(0x00);  /* 02 */
//     LCD_WR_DATA(0x00);  /* 02 */

//     LCD_WR_REG(0xF7); /* Resolution Control */
//     LCD_WR_DATA(0x82);  /* 480*800 */

//     LCD_WR_REG(0xB1); /* FRAME RATE Setting */
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x13);
//     LCD_WR_DATA(0x13); 

//     LCD_WR_REG(0XF2); /* CR_EQ_PC_SDT  #C0,06,40,28 */
//     LCD_WR_DATA(0x80);
//     LCD_WR_DATA(0x04);
//     LCD_WR_DATA(0x40);
//     LCD_WR_DATA(0x28);

//     LCD_WR_REG(0XC1); /* Power Control 2  SD OP Bias_VRH1_VRH2_EXT_CPCK_SEL */
//     LCD_WR_DATA(0x17);
//     LCD_WR_DATA(0x88);  /* VGMP */
//     LCD_WR_DATA(0x88);  /* VGMN */
//     LCD_WR_DATA(0x20);

//     LCD_WR_REG(0xE0); /* Positive Gamma Control */
//     LCD_WR_DATA(0x00);  /* P1 */
//     LCD_WR_DATA(0x0A);  /* P2 */
//     LCD_WR_DATA(0x12);  /* P3 */
//     LCD_WR_DATA(0x10);  /* P4 */
//     LCD_WR_DATA(0x0E);  /* P5 */
//     LCD_WR_DATA(0x20);  /* P6 */
//     LCD_WR_DATA(0xCC);  /* P7 */
//     LCD_WR_DATA(0x07);  /* P8 */
//     LCD_WR_DATA(0x06);  /* P9 */
//     LCD_WR_DATA(0x0B);  /* P10 */
//     LCD_WR_DATA(0x0E);  /* P11 */
//     LCD_WR_DATA(0x0F);  /* P12 */
//     LCD_WR_DATA(0x0D);  /* P13 */
//     LCD_WR_DATA(0x15);  /* P14 */
//     LCD_WR_DATA(0x10);  /* P15 */
//     LCD_WR_DATA(0x00);  /* P16 */

//     LCD_WR_REG(0xE1); /* Negative Gamma Correction */
//     LCD_WR_DATA(0x00);  /* P1 */
//     LCD_WR_DATA(0x0B);  /* P2 */
//     LCD_WR_DATA(0x13);  /* P3 */
//     LCD_WR_DATA(0x0D);  /* P4 */
//     LCD_WR_DATA(0x0E);  /* P5 */
//     LCD_WR_DATA(0x1B);  /* P6 */
//     LCD_WR_DATA(0x71);  /* P7 */
//     LCD_WR_DATA(0x06);  /* P8 */
//     LCD_WR_DATA(0x06);  /* P9 */
//     LCD_WR_DATA(0x0A);  /* P10 */
//     LCD_WR_DATA(0x0F);  /* P11 */
//     LCD_WR_DATA(0x0E);  /* P12 */
//     LCD_WR_DATA(0x0F);  /* P13 */
//     LCD_WR_DATA(0x15);  /* P14 */
//     LCD_WR_DATA(0x0C);  /* P15 */
//     LCD_WR_DATA(0x00);  /* P16 */

//     LCD_WR_REG(0x2a);   
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x01);
//     LCD_WR_DATA(0xdf);

//     LCD_WR_REG(0x2b);   
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x03);
//     LCD_WR_DATA(0x1f);

//     LCD_WR_REG(0x3A); /* Pixel Format */
//     LCD_WR_DATA(0x55);

//     LCD_WR_REG(0x36); /* Memory Access Control */
//     LCD_WR_DATA(0x00);  /* 02-180 */

//     LCD_WR_REG(0x11);
//     HAL_Delay(120);   
//     LCD_WR_REG(0x29);  
//     HAL_Delay(20);  
//     LCD_WR_REG(0x2C);
// }

// /**
//  * @brief       SSD1963寄存器初始化代码 
//  * @param       无
//  * @retval      无
//  */
// void LCD_IC_ssd1963_reginit(void)
// {
//     LCD_WR_REG(0xE2); /* Set PLL with OSC = 10MHz (hardware),	Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz */
//     LCD_WR_DATA(0x1D);  /* 参数1 */
//     LCD_WR_DATA(0x02);  /* 参数2 Divider M = 2, PLL = 300/(M+1) = 100MHz */
//     LCD_WR_DATA(0x04);  /* 参数3 Validate M and N values */
//     Delay_us(100);
//     LCD_WR_REG(0xE0); /*  Start PLL command */
//     LCD_WR_DATA(0x01);  /*  enable PLL */
//     HAL_Delay(10);
//     LCD_WR_REG(0xE0); /*  Start PLL command again */
//     LCD_WR_DATA(0x03);  /*  now, use PLL output as system clock */
//     HAL_Delay(12);
//     LCD_WR_REG(0x01); /* 软复位 */
//     HAL_Delay(10);

//     LCD_WR_REG(0xE6); /* 设置像素频率,33Mhz */
//     LCD_WR_DATA(0x2F);
//     LCD_WR_DATA(0xFF);
//     LCD_WR_DATA(0xFF);

//     LCD_WR_REG(0xB0); /* 设置LCD模式 */
//     LCD_WR_DATA(0x20);  /* 24位模式 */
//     LCD_WR_DATA(0x00);  /* TFT 模式 */

//     LCD_WR_DATA((SSD_HOR_RESOLUTION - 1) >> 8); /* 设置LCD水平像素 */
//     LCD_WR_DATA(SSD_HOR_RESOLUTION - 1);
//     LCD_WR_DATA((SSD_VER_RESOLUTION - 1) >> 8); /* 设置LCD垂直像素 */
//     LCD_WR_DATA(SSD_VER_RESOLUTION - 1);
//     LCD_WR_DATA(0x00);  /* RGB序列 */

//     LCD_WR_REG(0xB4); /* Set horizontal period */
//     LCD_WR_DATA((SSD_HT - 1) >> 8);
//     LCD_WR_DATA(SSD_HT - 1);
//     LCD_WR_DATA(SSD_HPS >> 8);
//     LCD_WR_DATA(SSD_HPS);
//     LCD_WR_DATA(SSD_HOR_PULSE_WIDTH - 1);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);
//     LCD_WR_REG(0xB6); /* Set vertical perio */
//     LCD_WR_DATA((SSD_VT - 1) >> 8);
//     LCD_WR_DATA(SSD_VT - 1);
//     LCD_WR_DATA(SSD_VPS >> 8);
//     LCD_WR_DATA(SSD_VPS);
//     LCD_WR_DATA(SSD_VER_FRONT_PORCH - 1);
//     LCD_WR_DATA(0x00);
//     LCD_WR_DATA(0x00);

//     LCD_WR_REG(0xF0); /* 设置SSD1963与CPU接口为16bit */
//     LCD_WR_DATA(0x03);  /* 16-bit(565 format) data for 16bpp */

//     LCD_WR_REG(0x29); /* 开启显示 */
//     /* 设置PWM输出  背光通过占空比可调 */
//     LCD_WR_REG(0xD0); /* 设置自动白平衡DBC */
//     LCD_WR_DATA(0x00);  /* disable */

//     LCD_WR_REG(0xBE); /* 配置PWM输出 */
//     LCD_WR_DATA(0x05);  /* 1设置PWM频率 */
//     LCD_WR_DATA(0xFE);  /* 2设置PWM占空比 */
//     LCD_WR_DATA(0x01);  /* 3设置C */
//     LCD_WR_DATA(0x00);  /* 4设置D */
//     LCD_WR_DATA(0x00);  /* 5设置E */
//     LCD_WR_DATA(0x00);  /* 6设置F */

//     LCD_WR_REG(0xB8); /* 设置GPIO配置 */
//     LCD_WR_DATA(0x03);  /* 2个IO口设置成输出 */
//     LCD_WR_DATA(0x01);  /* GPIO使用正常的IO功能 */
//     LCD_WR_REG(0xBA);
//     LCD_WR_DATA(0X01);  /* GPIO[1:0]=01,控制LCD方向 */
// }
