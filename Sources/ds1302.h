// ds1302.h
// 名称:DS1302 时钟芯片驱动程序头文件
#ifndef DS1302_H
#define DS1302_H

#include <reg51.h>    // 51 单片机寄存器定义
#include <intrins.h>  // 可能用到的 _nop_ 等指令（若驱动里有用到可保留）

// 类型定义
#define INT8U  unsigned char  
#define INT16U unsigned int  

// DS1302 硬件引脚定义（假设接在 P1 口，可根据实际电路修改）
sbit IO   = P1^0;  // 数据线
sbit SCLK = P1^1;  // 时钟线
sbit RST  = P1^2;  // 复位线

// 全局变量与函数声明 
extern INT8U DateTime[7];  // 存储读取的日期时间（年、月、日、时、分、秒、星期 等）

// 函数声明
void Write_A_Byte_TO_DS1302(INT8U x);  // 向 DS1302 写 1 字节
INT8U Get_A_Byte_FROM_DS1302(void);    // 从 DS1302 读 1 字节
INT8U Read_Data(INT8U addr);           // 从指定地址读数据
void GetDateTime(void);                // 读取完整日期时间到 DateTime

#endif 