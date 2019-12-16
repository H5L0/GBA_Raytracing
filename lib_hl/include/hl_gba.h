#pragma once

#define SCREEN_X		240		//��Ļ���
#define SCREEN_Y		160		//��Ļ�߶�
#define SC_WIDTH		SCREEN_X
#define SC_HEIGHT		SCREEN_Y
#define SC_W			SCREEN_X
#define SC_H			SCREEN_Y

/*----------------GBA�������ڴ��С----------------*/
#define SRAM_SIZE		0x00003FFF		//ϵͳ�ڴ� ���� (16 KB)
#define EWRAM_SIZE	0x0003FFFF		//CPU�ⲿ�����ڴ� ���� (256KB)
#define IWRAM_SIZE	0x00007FFF		//CPU�ڲ������ڴ� ���� (32KB)
#define REGRAM_SIZE	0x00003FFF		//�Ĵ����ڴ� ���� (16KB)
#define PALRAM_SIZE	0x000003FF		//��ɫ���ڴ� ���� (1KB)
#define VRAM_SIZE		0x00017FFF		//��Ƶ�ڴ� ���� (96KB)
#define OAM_SIZE		0x000003FF		//�����ڴ� ���� (1KB)

/*----------------GBA�������ڴ��ַ----------------*/
#ifdef WIN32
extern unsigned int gba_sram;
extern unsigned int gba_ewram;
extern unsigned int gba_iwram;
extern unsigned int gba_regram;
extern unsigned int gba_palram;
extern unsigned int gba_vram;
extern unsigned int gba_oam;
extern unsigned int gba_gprom;
#define SRAM			gba_sram		//ϵͳ�ڴ� ��ʼ��ַ (16 Kbytes) (14bit)
#define EWRAM			gba_ewram		//CPU�ⲿ�����ڴ� ��ʼ��ַ (256Kbytes) (18bit)
#define IWRAM			gba_iwram		//CPU�ڲ������ڴ� ��ʼ��ַ (32 Kbytes) (15bit)
#define REGRAM		gba_regram		//�Ĵ����ڴ� ��ʼ��ַ (? Kbyte)
#define PALRAM			gba_palram		//��ɫ���ڴ� ��ʼ��ַ (1 Kbyte) (10bit)
#define VRAM			gba_vram		//��Ƶ�ڴ� ��ʼ��ַ (96 Kbytes) (16.5bit)
#define OAM			gba_oam		//�����ڴ� ��ʼ��ַ (1 Kbyte) (10bit)
#define GPROM			gba_gprom		//��Ϸ��ֻ���ڴ� ��ʼ��ַ (���3x32M)
#else
#define SRAM			0x00000000		//ϵͳ�ڴ� ��ʼ��ַ (16 Kbytes) (14bit)
#define EWRAM			0x02000000		//CPU�ⲿ�����ڴ� ��ʼ��ַ (256Kbytes) (18bit)
#define IWRAM			0x03000000		//CPU�ڲ������ڴ� ��ʼ��ַ (32 Kbytes) (15bit)
#define REGRAM		0x04000000		//�Ĵ����ڴ� ��ʼ��ַ (? Kbyte)
#define PALRAM			0x05000000		//��ɫ���ڴ� ��ʼ��ַ (1 Kbyte) (10bit)
#define VRAM			0x06000000		//��Ƶ�ڴ� ��ʼ��ַ (96 Kbytes) (16.5bit)
#define OAM			0x07000000		//�����ڴ� ��ʼ��ַ (1 Kbyte) (10bit)
#define GPROM			0x08000000		//��Ϸ��ֻ���ڴ� ��ʼ��ַ (���3x32M)
#endif // WIN32_VIRTUAL




/*--------------------------LCD��ؼĴ���------------------------*/
//LCD״̬��Ϣ DisplayState �Ĵ�����ַ
//[15-8]��ֱ�����ȶ�ֵ 
//[5]��ֱ������ȶ�ֵ���ʱ�������ж�����
//[4]H-Blankʱ�������ж�����
//[3]V-Blankʱ�������ж�����
//[1]�Ƿ���H-Blank
//[0]�Ƿ���V-Blank��
#define REG_LCDSTATE	(REGRAM + 0x0004)
#define REG_DISPSTAT	REG_LCDSTATE

//��ֱ������ V Counter �Ĵ�����ַ
//ָʾLCD��Ⱦ����һ�У�0-159������Ⱦ��160-227��ֱ�հס�
#define REG_VCOUNT	(REGRAM + 0x0006)

/*--------------------------ͼ����ؼĴ���------------------------*/
//ͼ��ϵͳ����ָ�� �Ĵ�����ַ
//[15] OBJ Window Display Flag
//[14]Window0  [13] Window1 �Ƿ���ʾ
//[12]OBJ  [11]BG3  [10]BG2  [9]BG1  [8]BG0 �Ƿ���ʾ
//[7]ǿ�ƿհף�CPUֹͣͼ����
//[6]VRAM�о����ַ�ӳ�䷽ʽ��0����ά��1��һά
//[5]�Ƿ���H-Blank�ڼ���о��鴦��**
//[4]�趨��ǰ֡0/1��ģʽ4��5��
//[3]�Ƿ�GBCģʽ **ֻ��**
//[2-0]�趨6�ֱ���ģʽ 0-5
#define REG_DISPCNT	(REGRAM + 0x0000)
#define DISPCNT	(*(vu16*)REG_DISPCNT)


#define REG_BG0CNT	(REGRAM + 0x0008)		//����������0 �Ĵ�����ַ
#define REG_BG1CNT	(REGRAM + 0x000A)		//����������1 �Ĵ�����ַ
#define REG_BG2CNT	(REGRAM + 0x000C)		//����������2 �Ĵ�����ַ
#define REG_BG3CNT	(REGRAM + 0x000E)		//����������3 �Ĵ�����ַ
#define BG0CNT	(*(vu16*)REG_BG0CNT)		//����������0 ֵ
#define BG1CNT	(*(vu16*)REG_BG1CNT)		//����������1 ֵ
#define BG2CNT	(*(vu16*)REG_BG2CNT)		//����������2 ֵ
#define BG3CNT	(*(vu16*)REG_BG3CNT)		//����������3 ֵ

//�������趨 �Ĵ�����ַ
//OBJ [15-12]��ֱ [11-8]ˮƽ
//BG  [7-4]��ֱ [3-0]ˮƽ
#define REG_MOSAIC	(REGRAM + 0x004C)
#define MOSAIC	(*(vu16*)REG_MOSAIC)

/*-------------------------��ʱ����ؼĴ���-------------------------*/
//��ʱ�� ��ֵ�Ĵ���
//�趨Ϊ-n ��ζ�� 2^16 - n
#define REG_TIMER0_DATA	(REGRAM + 0x0100)		//��ʱ��0 �Ĵ�����ַ
#define REG_TIMER1_DATA	(REGRAM + 0x0104)		//��ʱ��1 �Ĵ�����ַ
#define REG_TIMER2_DATA	(REGRAM + 0x0108)		//��ʱ��2 �Ĵ�����ַ
#define REG_TIMER3_DATA	(REGRAM + 0x010C)		//��ʱ��3 �Ĵ�����ַ
#define TIMER0_DATA	(*(vu16*)REG_TIMER0_DATA)	//��ʱ��0 16λֵ
#define TIMER1_DATA	(*(vu16*)REG_TIMER1_DATA)	//��ʱ��1 16λֵ
#define TIMER2_DATA	(*(vu16*)REG_TIMER2_DATA)	//��ʱ��2 16λֵ
#define TIMER3_DATA	(*(vu16*)REG_TIMER3_DATA)	//��ʱ��3 16λֵ

//��ʱ���趨 �Ĵ�����ַ
//[0-1] 1 64 256 1024����
//[2]��������  [6]�����ж�
//[7]������ʱ��
#define REG_TIMER0_CNT	(REGRAM + 0x0102)		//��ʱ��0 �趨�Ĵ�����ַ
#define REG_TIMER1_CNT	(REGRAM + 0x0106)		//��ʱ��1 �趨�Ĵ�����ַ
#define REG_TIMER2_CNT	(REGRAM + 0x010A)		//��ʱ��2 �趨�Ĵ�����ַ
#define REG_TIMER3_CNT	(REGRAM + 0x010E)		//��ʱ��3 �趨�Ĵ�����ַ
//?16/8λ
#define TIMER0_CNT	(*(vu16*)REG_TIMER0_CNT)		//��ʱ��0 �趨�Ĵ���ֵ
#define TIMER1_CNT	(*(vu16*)REG_TIMER1_CNT)		//��ʱ��1 �趨�Ĵ���ֵ
#define TIMER2_CNT	(*(vu16*)REG_TIMER2_CNT)		//��ʱ��2 �趨�Ĵ���ֵ
#define TIMER3_CNT	(*(vu16*)REG_TIMER3_CNT)		//��ʱ��3 �趨�Ĵ���ֵ

//�������� �Ĵ�����ַ
#define REG_KEY_INPUT	(REGRAM + 0x0130)
//�������� ֵ
#define KEY_INPUT		(*(vu16*)REG_KEY_INPUT)
