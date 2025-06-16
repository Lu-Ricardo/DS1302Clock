// DS1302.c
#include "DS1302.h"
INT8U DateTime[7]; // ��������ȡ������ʱ������

// �� DS1302 д��һ�ֽ�
void Write_A_Byte_TO_DS1302(INT8U x) 
{
    INT8U i;
    for(i = 0x01; i != 0x00; i <<= 1) {
        IO = x & i; 
        SCLK = 1; 
        SCLK = 0;
    }
}

// �� DS1302 ��ȡһ�ֽ�
INT8U Get_A_Byte_FROM_DS1302() 
{
    INT8U i, dat = 0x00;
    for(i = 0x01; i != 0x00; i <<= 1) {
        if (IO) 
            dat |= i; 
        SCLK = 1; 
        SCLK = 0; 
    }
    return dat; // ��������ȡ�� BCD ��
    // ��Ҫ�� BCD ��ת��Ϊʮ���������أ���ʹ���������֮һ�滻 return dat;
    // return dat / 16 * 10 + dat % 16; 
    // return (dat >> 4) * 10 + (dat & 0x0F); 
    // ע�⣺ʹ�ò�ͬ���뷵��ʱ���������и�ʽ������ʱ�亯��Ҫ��Ӧ�����ֽ��߼�
}

// �� DS1302 ָ��λ�ö�����
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

// ��ȡ��ǰ����ʱ��
void GetDateTime() 
{
    INT8U i, addrc = 0x81;
    for (i = 0; i < 7; i++, addrc += 2 ) 
        DateTime[i] = Read_Data(addrc);
}