#pragma once
#include <hl_gba.h>
#include <hl_system.h>
#include <hl_math.h>

//������ʱ��
#define TM_ENABLE	0b10000000
//�����ж�
#define TM_INTERRUPT	0b01000000
//��������
#define TM_CASCADE	0b00000100

//1ʱ������	 16777216Hz 59.605ns
#define TM_CY1		0
//64ʱ������ 262144Hz 3.815us
#define TM_CY64		1
//256ʱ������ 65536Hz 15.259us
#define TM_CY256		2
//1024ʱ������ 16384Hz 61.035us
#define TM_CY1024		3


static inline void EnableTimer(int timerID)
{ *(vu16 *)(REG_TIMER0_CNT + timerID * 4) |= TM_ENABLE; }

static inline void DisableTimer(int timerID)
{ *(vu16 *)(REG_TIMER0_CNT + timerID * 4) &= ~TM_ENABLE; }


//�������ü�ʱ��������ʱ
static inline void ResetTestTimer()
{
	//Timer0��ʹ��
	//Timer1����Ϊ��ͣ����
	//Timer2 ��65536Hz���������Ƶ��1Hz
	//����������ͣ��������256���ڼ���
	TIMER2_DATA = 0;
	TIMER2_CNT = 0;
	TIMER2_CNT = TM_ENABLE | TM_CY256;
	//Timer3ÿ�������Լ18Сʱ12�����
	TIMER3_DATA = 0;
	TIMER3_CNT = 0;
	TIMER3_CNT = TM_ENABLE | TM_CASCADE;
}

//������ʱ��
static inline void StartTestTimer()
{
	TIMER2_CNT &= ~TM_CASCADE;
}

//��ͣ��ʱ��
static inline void PauseTestTimer()
{
	TIMER2_CNT |= TM_CASCADE;
}


static inline fp32 GetTestTimer()
{
	//��ȡ����ֵ�Ǵӳ�ʼֵ��ʼ��
	u16 tint = TIMER3_DATA;
	//ʱ��̰�����ʾ
	if (tint < FP32_MAXINT) return fp32_int(tint) + (((fp32)TIMER2_DATA) << (FP32_FBIT - 16));
	//ʱ�䳤��������ʾ
	int minute = tint / 60;
	return -(fp32_int(minute) + ((tint - minute * 60) << FP32_FBIT) / 60);
}