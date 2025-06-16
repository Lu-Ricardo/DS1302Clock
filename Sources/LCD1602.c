/***************   writer:shopping.w   ******************/
#include "LCD1602.h" // 包含头文件

// 毫秒延时函数实现
void Delay_ms(INT16U ms)
{
    INT8U i;
    while (ms--)
    {
        for (i = 0; i < 120; i++)
            ;
    }
}

// 忙状态检查函数实现
INT8U LCD_CheckBusy(void)
{
    INT8U status;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_EN = 1;
    Delay_ms(1);
    status = P0;
    LCD_EN = 0;
    return status;
}

// 写命令函数实现
void LCD_WriteCommand(INT8U cmd)
{
    while ((LCD_CheckBusy() & 0x80) == 0x80)
        ; // 等待不忙
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;
    P0 = cmd;
    LCD_EN = 1;
    Delay_ms(1);
    LCD_EN = 0;
}

// 写数据函数实现
void LCD_WriteData(INT8U dat)
{
    while ((LCD_CheckBusy() & 0x80) == 0x80)
        ; // 等待不忙
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_EN = 0;
    P0 = dat;
    LCD_EN = 1;
    Delay_ms(1);
    LCD_EN = 0;
}

// LCD初始化函数实现
void Initialize_LCD(void)
{
    LCD_WriteCommand(0x38); // 8位数据，2行显示，5x7点阵
    Delay_ms(1);
    LCD_WriteCommand(0x01); // 清屏
    Delay_ms(1);
    LCD_WriteCommand(0x06); // 写入新数据后光标右移
    Delay_ms(1);
    LCD_WriteCommand(0x0C); // 开显示，不显示光标
    Delay_ms(1);
}

// 显示字符串函数实现
void LCD_ShowString(INT8U row, INT8U col, INT8U *str)
{
    // 设置光标位置
    if (row == 0)
    {
        LCD_WriteCommand(0x80 | col);
    }
    else if (row == 1)
    {
        LCD_WriteCommand(0xC0 | col);
    }
    // 显示字符串（直到遇到'\0'）
    while (*str)
    {
        LCD_WriteData(*str++);
    }
}