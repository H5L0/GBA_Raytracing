#pragma once
#include <hl_gba.h>
#include <hl_system.h>
#include <hl_math.h>

//启动计时器
#define TM_ENABLE	0b10000000
//开启中断
#define TM_INTERRUPT	0b01000000
//开启级联
#define TM_CASCADE	0b00000100

//1时钟周期	 16777216Hz 59.605ns
#define TM_CY1		0
//64时钟周期 262144Hz 3.815us
#define TM_CY64		1
//256时钟周期 65536Hz 15.259us
#define TM_CY256		2
//1024时钟周期 16384Hz 61.035us
#define TM_CY1024		3


static inline void EnableTimer(int timerID)
{ *(vu16 *)(REG_TIMER0_CNT + timerID * 4) |= TM_ENABLE; }

static inline void DisableTimer(int timerID)
{ *(vu16 *)(REG_TIMER0_CNT + timerID * 4) &= ~TM_ENABLE; }


//重新设置计时器，并计时
static inline void ResetTestTimer()
{
	//Timer0不使用
	//Timer1保留为暂停控制
	//Timer2 以65536Hz计数，溢出频率1Hz
	//开启级联暂停，否则以256周期计数
	TIMER2_DATA = 0;
	TIMER2_CNT = 0;
	TIMER2_CNT = TM_ENABLE | TM_CY256;
	//Timer3每秒计数，约18小时12分溢出
	TIMER3_DATA = 0;
	TIMER3_CNT = 0;
	TIMER3_CNT = TM_ENABLE | TM_CASCADE;
}

//启动计时器
static inline void StartTestTimer()
{
	TIMER2_CNT &= ~TM_CASCADE;
}

//暂停计时器
static inline void PauseTestTimer()
{
	TIMER2_CNT |= TM_CASCADE;
}


static inline fp32 GetTestTimer()
{
	//读取的数值是从初始值开始的
	u16 tint = TIMER3_DATA;
	//时间短按秒显示
	if (tint < FP32_MAXINT) return fp32_int(tint) + (((fp32)TIMER2_DATA) << (FP32_FBIT - 16));
	//时间长按分钟显示
	int minute = tint / 60;
	return -(fp32_int(minute) + ((tint - minute * 60) << FP32_FBIT) / 60);
}