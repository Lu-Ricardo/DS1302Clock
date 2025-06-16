/***************   writer:shopping.w   ******************/
#ifndef __LCD1602_H__
#define __LCD1602_H__

#include <reg51.h>
#include <intrins.h>

#define INT8U unsigned char
#define INT16U unsigned int

// LCD1602引脚定义（与main.c保持一致）
sbit LCD_RS = P2^0;  // RS控制引脚
sbit LCD_RW = P2^1;  // RW控制引脚
sbit LCD_EN = P2^2;  // EN控制引脚

// 函数声明
void Initialize_LCD(void);          // LCD初始化
void LCD_ShowString(INT8U row, INT8U col, INT8U *str);  // 显示字符串
void LCD_WriteCommand(INT8U cmd);   // 写命令
void LCD_WriteData(INT8U dat);      // 写数据
INT8U LCD_CheckBusy(void);          // 忙状态检查
void Delay_ms(INT16U ms);           // 毫秒延时

#endif // __LCD1602_H__