#include "stm32h7xx.h"                  // Device header

#include <stdio.h>
#include "LCD_IC_Init.h"
#include "LCD_Init.h"
#include "LCD.h"

//LCD的画笔颜色和背景色
uint32_t POINT_COLOR = 0XF800;	/* 画笔颜色 */
uint32_t BACK_COLOR  = WHITE;  /* 背景色 */

/* 管理LCD重要参数 */
_lcd_dev lcddev;

/**
 * @brief       LCD写数据
 * @param       data: 要写入的数据
 * @retval      无
 */
void LCD_WR_DATA(volatile uint16_t data)
{	  
	data=data;			/* 使用-O2优化的时候,必须插入的延时 */
	LCD->LCD_RAM=data;		 
}

/**
 * @brief       LCD写寄存器编号/地址函数
 * @param       regno: 寄存器编号/地址
 * @retval      无
 */
void LCD_WR_REG(volatile uint16_t regno)
{   
	regno=regno;		/* 使用-O2优化的时候,必须插入的延时 */
	LCD->LCD_REG=regno;/* 写入要写的寄存器序号 */ 
}

/**
 * @brief       LCD写寄存器
 * @param       regno:寄存器编号/地址
 * @param       data:要写入的数据
 * @retval      无
 */
void LCD_WriteReg(uint16_t regno,uint16_t data)
{	
	LCD->LCD_REG = regno;	/* 写入要写的寄存器序号 */
	LCD->LCD_RAM = data;	/* 写入数据 */    		 
}

/**
 * @brief       LCD延时函数,仅用于部分在mdk -O1时间优化时需要设置的地方
 * @param       t:延时的数值
 * @retval      无
 */
static void LCD_opt_delay(uint32_t i)
{
    while (i--);
}

/**
 * @brief       LCD读数据
 * @param       无
 * @retval      读取到的数据
 */
static uint16_t LCD_RD_DATA(void)
{
    volatile uint16_t ram;  /* 防止被优化 */
    LCD_opt_delay(2);
    ram = LCD->LCD_RAM;
    return ram;
}

/**
 * @brief       准备写GRAM
 * @param       无
 * @retval      无
 */
void LCD_Write_RAM_Prepare(void)
{
	LCD->LCD_REG = lcddev.wramcmd;	  
}

/**
 * @brief       设置光标位置(对RGB屏无效)
 * @param       x,y: 坐标
 * @retval      无
 */
void LCD_Set_Cursor(uint16_t x, uint16_t y)
{
    // if (lcddev.id == 0X1963)
    // {
    //     if (lcddev.dir == 0)    /* 竖屏模式, x坐标需要变换 */
    //     {
    //         x = lcddev.width - 1 - x;
    //         LCD_WR_REG(lcddev.setxcmd);
    //         LCD_WR_DATA(0);
    //         LCD_WR_DATA(0);
    //         LCD_WR_DATA(x >> 8);
    //         LCD_WR_DATA(x & 0XFF);
    //     }
    //     else                    /* 横屏模式 */
    //     {
    //         LCD_WR_REG(lcddev.setxcmd);
    //         LCD_WR_DATA(x >> 8);
    //         LCD_WR_DATA(x & 0XFF);
    //         LCD_WR_DATA((lcddev.width - 1) >> 8);
    //         LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    //     }

    //     LCD_WR_REG(lcddev.setycmd);
    //     LCD_WR_DATA(y >> 8);
    //     LCD_WR_DATA(y & 0XFF);
    //     LCD_WR_DATA((lcddev.height - 1) >> 8);
    //     LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    // }

    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(x >> 8);
    LCD_WR_REG(lcddev.setxcmd + 1);
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(y >> 8);
    LCD_WR_REG(lcddev.setycmd + 1);
    LCD_WR_DATA(y & 0XFF);

    // else    /* 9341/5310/7789/7796/9806 等 设置坐标 */
    // {
    //     LCD_WR_REG(lcddev.setxcmd);
    //     LCD_WR_DATA(x >> 8);
    //     LCD_WR_DATA(x & 0XFF);
    //     LCD_WR_REG(lcddev.setycmd);
    //     LCD_WR_DATA(y >> 8);
    //     LCD_WR_DATA(y & 0XFF);
    // }
}

/**
 * @brief       读取个某点的颜色值
 * @param       x,y:坐标
 * @retval      此点的颜色(32位颜色,方便兼容LTDC)
 */
uint32_t LCD_Read_Point(uint16_t x, uint16_t y)
{
    uint16_t r = 0, g = 0, b = 0;

    if (x >= lcddev.width || y >= lcddev.height) return 0;  /* 超过了范围,直接返回 */

    LCD_Set_Cursor(x, y);       /* 设置坐标 */

    LCD_WR_REG(0X2E00);   /* 5510 发送读GRAM指令 */

    r = LCD_RD_DATA();          /* 假读(dummy read) */

    r = LCD_RD_DATA();          /* 实际坐标颜色 */
    
    /* 9341/5310/5510/7789/9806 要分2次读出 */
    b = LCD_RD_DATA();
    g = r & 0XFF;               /* 对于 9341/5310/5510/7789/9806, 第一次读取的是RG的值,R在前,G在后,各占8位 */
    g <<= 8;
    
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  /* 9341/5310/5510/7789/9806 需要公式转换一下 */
}

/**
 * @brief       LCD开启显示
 * @param       无
 * @retval      无
 */
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0X2900);   /* 开启显示 */
}

/**
 * @brief       LCD关闭显示
 * @param       无
 * @retval      无
 */
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0X2800);   /* 关闭显示 */
}

/**
 * @brief       设置LCD的自动扫描方向(对RGB屏无效)
 *   @note
 *              9341/5310/5510/1963/7789/7796/9806等IC已经实际测试
 *              注意:其他函数可能会受到此函数设置的影响(尤其是9341),
 *              所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
 *
 * @param       dir:0~7,代表8个方向(具体定义见lcd.h)
 * @retval      无
 */
void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;

    /* 横屏时，对1963不改变扫描方向, 其他IC改变扫描方向！竖屏时1963改变方向, 其他IC不改变扫描方向 */
    if ((lcddev.dir == 1 && lcddev.id != 0X1963) || (lcddev.dir == 0 && lcddev.id == 0X1963))
    {
        switch (dir)   /* 方向转换 */
        {
            case 0:
                dir = 6;
                break;

            case 1:
                dir = 7;
                break;

            case 2:
                dir = 4;
                break;

            case 3:
                dir = 5;
                break;

            case 4:
                dir = 1;
                break;

            case 5:
                dir = 0;
                break;

            case 6:
                dir = 3;
                break;

            case 7:
                dir = 2;
                break;
        }
    }
 
    /* 根据扫描方式 设置 0X36/0X3600 寄存器 bit 5,6,7 位的值 */
    switch (dir)
    {
        case L2R_U2D:   /* 从左到右,从上到下 */
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U:   /* 从左到右,从下到上 */
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D:   /* 从右到左,从上到下 */
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U:   /* 从右到左,从下到上 */
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R:   /* 从上到下,从左到右 */
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L:   /* 从上到下,从右到左 */
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R:   /* 从下到上,从左到右 */
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L:   /* 从下到上,从右到左 */
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    dirreg = 0X36;  /* 对绝大部分驱动IC, 由0X36寄存器控制 */

    dirreg = 0X3600;    /* 对于5510, 和其他驱动ic的寄存器有差异 */

    LCD_WriteReg(dirreg, regval);

    if (lcddev.id != 0X1963)   /* 1963不做坐标处理 */
    {
        if (regval & 0X20)
        {
            if (lcddev.width < lcddev.height)   /* 交换X,Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   /* 交换X,Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }

    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0);
    LCD_WR_REG(lcddev.setxcmd + 1);
    LCD_WR_DATA(0);
    LCD_WR_REG(lcddev.setxcmd + 2);
    LCD_WR_DATA((lcddev.width - 1) >> 8);
    LCD_WR_REG(lcddev.setxcmd + 3);
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0);
    LCD_WR_REG(lcddev.setycmd + 1);
    LCD_WR_DATA(0);
    LCD_WR_REG(lcddev.setycmd + 2);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_REG(lcddev.setycmd + 3);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

/**
 * @brief       SSD1963背光亮度设置函数
 * @param       pwm: 背光等级,0~100.越大越亮.
 * @retval      无
 */
void LCD_SSD_BackLight_Set(uint8_t pwm)
{
	LCD_WR_REG(0xBE);         /* 配置PWM输出 */
    LCD_WR_DATA(0x05);          /* 1设置PWM频率 */
    LCD_WR_DATA(pwm * 2.55);    /* 2设置PWM占空比 */
    LCD_WR_DATA(0x01);          /* 3设置C */
    LCD_WR_DATA(0xFF);          /* 4设置D */
    LCD_WR_DATA(0x00);          /* 5设置E */
    LCD_WR_DATA(0x00);          /* 6设置F */
}

/**
 * @brief       设置LCD显示方向
 * @param       dir:0,竖屏; 1,横屏
 * @retval      无
 */
void LCD_Display_Dir(uint8_t dir)
{
    lcddev.dir = dir;   /* 竖屏/横屏 */

    if (dir == 0)       /* 竖屏 */
    {
        lcddev.width = 240;
        lcddev.height = 320;

        lcddev.wramcmd = 0X2C00;
        lcddev.setxcmd = 0X2A00;
        lcddev.setycmd = 0X2B00;
        lcddev.width = 480;
        lcddev.height = 800;   
    }
    else                /* 横屏 */
    {
        lcddev.width = 320;         /* 默认宽度 */
        lcddev.height = 240;        /* 默认高度 */

        lcddev.wramcmd = 0X2C00;
        lcddev.setxcmd = 0X2A00;
        lcddev.setycmd = 0X2B00;
        lcddev.width = 800;
        lcddev.height = 480;
    }

    LCD_Scan_Dir(DFT_SCAN_DIR);     /* 默认扫描方向 */
}

/**
 * @brief       设置窗口(对RGB屏无效),并自动设置画点坐标到窗口左上角(sx,sy).
 * @param       sx,sy:窗口起始坐标(左上角)
 * @param       width,height:窗口宽度和高度,必须大于0!!
 *   @note      窗体大小:width*height.
 *
 * @retval      无
 */
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_REG(lcddev.setxcmd + 1);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_REG(lcddev.setxcmd + 2);
    LCD_WR_DATA(twidth >> 8);
    LCD_WR_REG(lcddev.setxcmd + 3);
    LCD_WR_DATA(twidth & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_REG(lcddev.setycmd + 1);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_REG(lcddev.setycmd + 2);
    LCD_WR_DATA(theight >> 8);
    LCD_WR_REG(lcddev.setycmd + 3);
    LCD_WR_DATA(theight & 0XFF);
}

/**
 * @brief       初始化LCD
 *   @note      该初始化函数可以初始化各种型号的LCD
 *
 * @param       无
 * @retval      无
 */

SRAM_HandleTypeDef g_sram_handle;

void LCD_Init(void)
{	
    LCD_opt_delay(0X1FFFF);     /* 初始化FMC后,必须等待一定时间才能开始初始化 */

    /* 尝试9341 ID的读取 */
    LCD_WR_REG(0XD3);
    lcddev.id = LCD_RD_DATA();  /* dummy read */
    lcddev.id = LCD_RD_DATA();  /* 读到0X00 */
    lcddev.id = LCD_RD_DATA();  /* 读取0X93 */
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA(); /* 读取0X41 */

    if (lcddev.id != 0X9341)    /* 不是 9341 , 尝试看看是不是 ST7789 */
    {
        LCD_WR_REG(0X04);
        lcddev.id = LCD_RD_DATA();      /* dummy read */
        lcddev.id = LCD_RD_DATA();      /* 读到0X85 */
        lcddev.id = LCD_RD_DATA();      /* 读取0X85 */
        lcddev.id <<= 8;
        lcddev.id |= LCD_RD_DATA();     /* 读取0X52 */

        if (lcddev.id != 0x7789)        /* 也不是ST7789, 尝试是不是 NT35310 */
        {
            LCD_WR_REG(0XD4);
            lcddev.id = LCD_RD_DATA();  /* dummy read */
            lcddev.id = LCD_RD_DATA();  /* 读回0X01 */
            lcddev.id = LCD_RD_DATA();  /* 读回0X53 */
            lcddev.id <<= 8;
            lcddev.id |= LCD_RD_DATA(); /* 这里读回0X10 */

            if (lcddev.id != 0X5310)    /* 也不是NT35310,尝试看看是不是ST7796 */
            {
                LCD_WR_REG(0XD3);
                lcddev.id = LCD_RD_DATA();  /* dummy read */
                lcddev.id = LCD_RD_DATA();  /* 读到0X00 */
                lcddev.id = LCD_RD_DATA();  /* 读取0X77 */
                lcddev.id <<= 8;
                lcddev.id |= LCD_RD_DATA(); /* 读取0X96 */
                
                if (lcddev.id != 0x7796)    /* 也不是ST7796,尝试看看是不是NT35510 */
                {
                    /* 发送秘钥（厂家提供,照搬即可） */
                    LCD_WriteReg(0xF000, 0x0055);
                    LCD_WriteReg(0xF001, 0x00AA);
                    LCD_WriteReg(0xF002, 0x0052);
                    LCD_WriteReg(0xF003, 0x0008);
                    LCD_WriteReg(0xF004, 0x0001);
                    
                    LCD_WR_REG(0xC500);           /* 读取ID高8位 */
                    lcddev.id = LCD_RD_DATA();      /* 读回0X55 */
                    lcddev.id <<= 8;

                    LCD_WR_REG(0xC501);           /* 读取ID低8位 */
                    lcddev.id |= LCD_RD_DATA();     /* 读回0X10 */

                    HAL_Delay(5);                    /* 等待5ms, 因为0XC501指令对1963来说就是软件复位指令,等待5ms让1963复位OK再操作 */
                }
            }
        }
    }
	printf("LCD ID:%x\r\n", lcddev.id); /* LCD ID */
    LCD_IC_nt35510_reginit();   /* 执行NT35510初始化 */
    // if (lcddev.id == 0X7789)
    // {
    //     LCD_IC_st7789_reginit();    /* 执行ST7789初始化 */
    // }
    // else if (lcddev.id == 0X9341)
    // {
    //     LCD_IC_ili9341_reginit();   /* 执行ILI9341初始化 */
    // }
    // else if (lcddev.id == 0x5310)
    // {
    //     LCD_IC_nt35310_reginit();   /* 执行NT35310初始化 */
    // }
    // else if (lcddev.id == 0x7796)
    // {
    //     LCD_IC_st7796_reginit();    /* 执行ST7796初始化 */
    // }

    // else if (lcddev.id == 0x9806)
    // {
    //     LCD_IC_ili9806_reginit();   /* 执行ILI9806初始化 */
    // }
    // else if (lcddev.id == 0X1963)
    // {
    //     LCD_IC_ssd1963_reginit();   /* 执行SSD1963初始化 */
    //     LCD_SSD_BackLight_Set(100); /* 背光设置为最亮 */
    // }

    /* 由于不同屏幕的写时序不同，这里的时序可以根据自己的屏幕进行修改
      （若插上长排线对时序也会有影响，需要自己根据情况修改） */
    /* 初始化完成以后,提速 */
    /* 重新配置写时序控制寄存器的时序 */
	FMC_NORSRAM_TimingTypeDef fsmc_write_handle;

	fsmc_write_handle.AddressSetupTime = 2;
	fsmc_write_handle.DataSetupTime = 2;
	FMC_NORSRAM_Extended_Timing_Init(g_sram_handle.Extended, &fsmc_write_handle, g_sram_handle.Init.NSBank, g_sram_handle.Init.ExtendedMode);

    LCD_Display_Dir(LCD_DIR); /* 默认为竖屏 */
    /* 硬件已经上拉背光引脚电平 */
    LCD_Clear(WHITE);
}
