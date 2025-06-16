// DS1302.c
#include "DS1302.h"
INT8U DateTime[7]; // 保存所读取的日期时间数据

// 向 DS1302 写入一字节
void Write_A_Byte_TO_DS1302(INT8U x) 
{
    INT8U i;
    for(i = 0x01; i != 0x00; i <<= 1) {
        IO = x & i; 
        SCLK = 1; 
        SCLK = 0;
    }
}

// 从 DS1302 读取一字节
INT8U Get_A_Byte_FROM_DS1302() 
{
    INT8U i, dat = 0x00;
    for(i = 0x01; i != 0x00; i <<= 1) {
        if (IO) 
            dat |= i; 
        SCLK = 1; 
        SCLK = 0; 
    }
    return dat; // 返回所读取的 BCD 码
    // 若要将 BCD 码转换为十进制数返回，可使用下面语句之一替换 return dat;
    // return dat / 16 * 10 + dat % 16; 
    // return (dat >> 4) * 10 + (dat & 0x0F); 
    // 注意：使用不同编码返回时，主程序中格式化日期时间函数要对应调整分解逻辑
}

// 从 DS1302 指定位置读数据
INT8U Read_Data(INT8U addr) 
{
    INT8U dat;
    RST = 0; 
    SCLK = 0; 
    RST = 1;
    Write_A_Byte_TO_DS1302(addr); 
    dat = Get_A_Byte_FROM_DS1302();
    SCLK = 1; 
    RST = 0;
    return dat;
}

// 读取当前日期时间
void GetDateTime() 
{
    INT8U i, addrc = 0x81;
    for (i = 0; i < 7; i++, addrc += 2 ) 
        DateTime[i] = Read_Data(addrc);
}