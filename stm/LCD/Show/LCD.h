#ifndef __LCD_H__
#define __LCD_H__

#include "stm32h7xx.h"                  // Device header

void LCD_Clear(uint16_t color);	/* LCD清屏 */
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);/* 纯色填充矩形(32位颜色,兼容LTDC) */
void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);/* 彩色填充矩形 */
void LCD_Draw_Point(uint16_t x, uint16_t y, uint32_t color);	/* 画点(32位颜色,兼容LTDC) */
void LCD_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);/* 画直线 */
void LCD_Draw_HLine(uint16_t x, uint16_t y, uint16_t len, uint16_t color);/* 画水平线 */
void LCD_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);/* 画矩形 */
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);/* 画圆 */
void LCD_Fill_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);/* 填充实心圆 */
void LCD_Show_Char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);/* 显示一个字符 */
void LCD_Show_Num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);/* 显示数字 */
void LCD_Show_Xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);/* 扩展显示数字 */
void LCD_Show_String(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color); /* 显示字符串 */

#endif
