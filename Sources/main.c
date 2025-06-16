//--------------------------- main.c ---------------------------
// 名称: DS1302 实时时钟显示程序
// 说明: 从 DS1302 中读取时钟数据,在 LCD 上显示日期和时间。
//---------------------------------------------------------------
#include <reg51.h>
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include "LCD1602.h"
#include "ds1302.h"

#define ulong unsigned long
#define BUTTON_NUM 4
#define BUZZER_SOUND_COUNT 3

void Timer0_Init();
void Timer0_ISR();
void scanButton();
void buttonInit();

sbit btn1 = P3 ^ 0;
sbit btn2 = P3 ^ 1;
sbit btn3 = P3 ^ 2;
sbit btn4 = P3 ^ 3;
sbit buzzer = P3 ^ 4;

INT16U timer_count = 0;  // 中断计数（10ms×100=1秒）
bit one_second_flag = 0; // 1秒标志位

/*声明新的数据类型来记录按钮，其中包括
 *引脚电平
 *是否按下
 *是否释放
 */
typedef struct
{
    INT8U pin;
    int Press;
    int Released;
} ButtonState;

ButtonState btns[BUTTON_NUM];

int timer_isPause = 1;
int buz_sound;
ulong alarm_time = 0;
enum WorkMode
{
    NORMAL,
    TIMER,
    ALARM_CLOCK
};
// 0～6 分别对应周日,周一至周六
// 0～6 分别对应周日,周一至周六
// 0～6 分别对应周日,周一至周六
const char *WEEK[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
INT8U LCD_BUF[17];
INT8U ALARM[17];

enum WorkMode current_workmode = NORMAL;

void GetDateTime();                         // 读取当前日期时间
void Initialize_LCD();                      // LCD 初始化
void LCD_ShowString(INT8U, INT8U, INT8U *); // 在 LCD 上显示字符串

// 日期与时间值转换为数字符
void Format_DateTime(INT8U d, INT8U *a)
{
    *a = (d >> 4) + '0';
    *(a + 1) = (d & 0x0F) + '0'; //[1]
}

void main()
{
    ulong timer_start = 0;
    buzzer = 0;

    Initialize_LCD(); // 液晶屏初始化
    Timer0_Init();
    buttonInit();
    while (1)
    {
        GetDateTime(); // 从 DS1302 读取日期时间

        scanButton();
        // 按下按钮1，切换模式
        if (btns[0].Released == 1)
        {
            Initialize_LCD();
            current_workmode = (current_workmode + 1) % 3;
        }

        if (btns[3].Released == 1 && buz_sound == 1)
            buz_sound = 0;

        if (buz_sound == 1)
        {
            buzzer = 1;
            Delay_ms(500);
            buzzer = 0;
            Delay_ms(300);
        }

        switch (current_workmode)
        {
        case NORMAL:
            strcpy(&LCD_BUF, "DATE 00-00-00   ");
            // 格式化年月日
            Format_DateTime(DateTime[6], LCD_BUF + 5);
            Format_DateTime(DateTime[4], LCD_BUF + 8);
            Format_DateTime(DateTime[3], LCD_BUF + 11);

            // 格式化星期
            strcpy(LCD_BUF + 13, WEEK[DateTime[5] - 1]);

            // 显示年月日,星期,时分秒
            LCD_ShowString(0, 0, LCD_BUF);
            strcpy(&LCD_BUF, "TIME  00:00:00");
            // 格式化时分秒
            Format_DateTime(DateTime[2], LCD_BUF + 6);
            Format_DateTime(DateTime[1], LCD_BUF + 9);
            Format_DateTime(DateTime[0], LCD_BUF + 12);
            LCD_ShowString(1, 0, LCD_BUF);
            // 检测是否到闹钟时间
            if (alarm_time != 0)
            {
                strcpy(&ALARM, "TIME  00:00:00");
                sprintf(ALARM + 6, "%02d:%02d:%02d", (INT16U)alarm_time / 3600, (INT16U)alarm_time / 60 % 60, (INT16U)alarm_time % 60);
                if (strcmp(ALARM, LCD_BUF) == 0)
                {
                    buz_sound = 1;
                }
            }
            break;

        case ALARM_CLOCK:
            if (btns[1].Released == 1)
            {
                alarm_time = alarm_time + 3600;
            }

            if (btns[2].Released == 1)
            {
                alarm_time = alarm_time + 60;
            }
            if (alarm_time >= 86400)
                alarm_time = 0;
            strcpy(&LCD_BUF, "TIME  00:00:00");
            sprintf(LCD_BUF + 6, "%02d:%02d:%02d", (INT16U)alarm_time / 3600, (INT16U)alarm_time / 60 % 60, (INT16U)alarm_time % 60);
            LCD_ShowString(0, 0, "ALARMTIME");
            LCD_ShowString(1, 0, LCD_BUF);
            break;

        case TIMER:

            // 计时器状态下按下按钮3，暂停计时
            if (btns[2].Released == 1)
                timer_isPause = (timer_isPause == 0) ? 1 : 0;

            if (timer_isPause != 1)
            {
                strcpy(LCD_BUF, "      ");
                if (one_second_flag)
                {
                    one_second_flag = 0;
                    timer_start++;
                }
            }
            else
            {
                strcpy(&LCD_BUF, "Pause ");
            }

            if (btns[1].Released == 1 && timer_isPause == 1)
                timer_start = 0;

            LCD_ShowString(0, 0, "TIMER MODE"); // 显示模式指示
            sprintf(LCD_BUF + 6, "%02d:%02d:%02d", (INT16U)timer_start / 3600, (INT16U)timer_start / 60 % 60, (INT16U)timer_start % 60);
            LCD_ShowString(1, 0, LCD_BUF); // 在第2行显示时间
            break;
        }
    }
}

// 按钮检测函数
void scanButton()
{
    int i;
    // 每次都获取对应按键的值
    btns[0].pin = btn1;
    btns[1].pin = btn2;
    btns[2].pin = btn3;
    btns[3].pin = btn4;
    //   将Released设为0确保按下-释放的过程闭环
    for (i = 0; i < BUTTON_NUM; i++)
        btns[i].Released = 0;

    for (i = 0; i < BUTTON_NUM; i++)
    {
        if (btns[i].Press == 0 && btns[i].pin == 0)
            btns[i].Press = 1;
        if (btns[i].Press == 1 && btns[i].pin == 1)
        {
            btns[i].Press = 0;
            btns[i].Released = 1;
        }
    }
}

// 初始化按钮
void buttonInit()
{
    int i;
    for (i = 0; i < BUTTON_NUM; i++)
    {
        btns[i].Press = 0;
        btns[i].Released = 0;
    }
}

// 定时器0初始化（10ms中断，33MHz晶振）
void Timer0_Init()
{
    TMOD &= 0xF0; // 清除定时器0模式位
    TMOD |= 0x01; // 设置定时器0为16位模式
    TH0 = 0x94;   // 10ms初值（33MHz晶振）
    TL0 = 0xD4;
    ET0 = 1; // 使能定时器0中断
    EA = 1;  // 开总中断
    TR0 = 1; // 启动定时器0
}

// 定时器0中断服务函数
void Timer0_ISR() interrupt 1
{
    TH0 = 0x94; // 重新加载初值
    TL0 = 0xD4;
    timer_count++; // 中断计数加1

    if (timer_count >= 100)
    {                        // 100次×10ms=1秒
        timer_count = 0;     // 重置计数器
        one_second_flag = 1; // 置1秒标志
    }
}
