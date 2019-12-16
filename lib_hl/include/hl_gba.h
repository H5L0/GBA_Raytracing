#pragma once

#define SCREEN_X		240		//屏幕宽度
#define SCREEN_Y		160		//屏幕高度
#define SC_WIDTH		SCREEN_X
#define SC_HEIGHT		SCREEN_Y
#define SC_W			SCREEN_X
#define SC_H			SCREEN_Y

/*----------------GBA各部分内存大小----------------*/
#define SRAM_SIZE		0x00003FFF		//系统内存 容量 (16 KB)
#define EWRAM_SIZE	0x0003FFFF		//CPU外部工作内存 容量 (256KB)
#define IWRAM_SIZE	0x00007FFF		//CPU内部工作内存 容量 (32KB)
#define REGRAM_SIZE	0x00003FFF		//寄存器内存 容量 (16KB)
#define PALRAM_SIZE	0x000003FF		//调色盘内存 容量 (1KB)
#define VRAM_SIZE		0x00017FFF		//视频内存 容量 (96KB)
#define OAM_SIZE		0x000003FF		//物体内存 容量 (1KB)

/*----------------GBA各部分内存地址----------------*/
#ifdef WIN32
extern unsigned int gba_sram;
extern unsigned int gba_ewram;
extern unsigned int gba_iwram;
extern unsigned int gba_regram;
extern unsigned int gba_palram;
extern unsigned int gba_vram;
extern unsigned int gba_oam;
extern unsigned int gba_gprom;
#define SRAM			gba_sram		//系统内存 起始地址 (16 Kbytes) (14bit)
#define EWRAM			gba_ewram		//CPU外部工作内存 起始地址 (256Kbytes) (18bit)
#define IWRAM			gba_iwram		//CPU内部工作内存 起始地址 (32 Kbytes) (15bit)
#define REGRAM		gba_regram		//寄存器内存 起始地址 (? Kbyte)
#define PALRAM			gba_palram		//调色板内存 起始地址 (1 Kbyte) (10bit)
#define VRAM			gba_vram		//视频内存 起始地址 (96 Kbytes) (16.5bit)
#define OAM			gba_oam		//物体内存 起始地址 (1 Kbyte) (10bit)
#define GPROM			gba_gprom		//游戏卡只读内存 起始地址 (最大3x32M)
#else
#define SRAM			0x00000000		//系统内存 起始地址 (16 Kbytes) (14bit)
#define EWRAM			0x02000000		//CPU外部工作内存 起始地址 (256Kbytes) (18bit)
#define IWRAM			0x03000000		//CPU内部工作内存 起始地址 (32 Kbytes) (15bit)
#define REGRAM		0x04000000		//寄存器内存 起始地址 (? Kbyte)
#define PALRAM			0x05000000		//调色板内存 起始地址 (1 Kbyte) (10bit)
#define VRAM			0x06000000		//视频内存 起始地址 (96 Kbytes) (16.5bit)
#define OAM			0x07000000		//物体内存 起始地址 (1 Kbyte) (10bit)
#define GPROM			0x08000000		//游戏卡只读内存 起始地址 (最大3x32M)
#endif // WIN32_VIRTUAL




/*--------------------------LCD相关寄存器------------------------*/
//LCD状态信息 DisplayState 寄存器地址
//[15-8]垂直计数比对值 
//[5]垂直计数与比对值相等时，生成中断请求
//[4]H-Blank时，生成中断请求
//[3]V-Blank时，生成中断请求
//[1]是否在H-Blank
//[0]是否在V-Blank。
#define REG_LCDSTATE	(REGRAM + 0x0004)
#define REG_DISPSTAT	REG_LCDSTATE

//垂直计数器 V Counter 寄存器地址
//指示LCD渲染到哪一行，0-159正在渲染，160-227垂直空白。
#define REG_VCOUNT	(REGRAM + 0x0006)

/*--------------------------图像相关寄存器------------------------*/
//图像系统设置指令 寄存器地址
//[15] OBJ Window Display Flag
//[14]Window0  [13] Window1 是否显示
//[12]OBJ  [11]BG3  [10]BG2  [9]BG1  [8]BG0 是否显示
//[7]强制空白，CPU停止图像处理
//[6]VRAM中精灵字符映射方式，0：二维，1：一维
//[5]是否在H-Blank期间进行精灵处理**
//[4]设定当前帧0/1（模式4和5）
//[3]是否GBC模式 **只读**
//[2-0]设定6种背景模式 0-5
#define REG_DISPCNT	(REGRAM + 0x0000)
#define DISPCNT	(*(vu16*)REG_DISPCNT)


#define REG_BG0CNT	(REGRAM + 0x0008)		//背景控制器0 寄存器地址
#define REG_BG1CNT	(REGRAM + 0x000A)		//背景控制器1 寄存器地址
#define REG_BG2CNT	(REGRAM + 0x000C)		//背景控制器2 寄存器地址
#define REG_BG3CNT	(REGRAM + 0x000E)		//背景控制器3 寄存器地址
#define BG0CNT	(*(vu16*)REG_BG0CNT)		//背景控制器0 值
#define BG1CNT	(*(vu16*)REG_BG1CNT)		//背景控制器1 值
#define BG2CNT	(*(vu16*)REG_BG2CNT)		//背景控制器2 值
#define BG3CNT	(*(vu16*)REG_BG3CNT)		//背景控制器3 值

//马赛克设定 寄存器地址
//OBJ [15-12]垂直 [11-8]水平
//BG  [7-4]垂直 [3-0]水平
#define REG_MOSAIC	(REGRAM + 0x004C)
#define MOSAIC	(*(vu16*)REG_MOSAIC)

/*-------------------------计时器相关寄存器-------------------------*/
//计时器 数值寄存器
//设定为-n 意味着 2^16 - n
#define REG_TIMER0_DATA	(REGRAM + 0x0100)		//计时器0 寄存器地址
#define REG_TIMER1_DATA	(REGRAM + 0x0104)		//计时器1 寄存器地址
#define REG_TIMER2_DATA	(REGRAM + 0x0108)		//计时器2 寄存器地址
#define REG_TIMER3_DATA	(REGRAM + 0x010C)		//计时器3 寄存器地址
#define TIMER0_DATA	(*(vu16*)REG_TIMER0_DATA)	//计时器0 16位值
#define TIMER1_DATA	(*(vu16*)REG_TIMER1_DATA)	//计时器1 16位值
#define TIMER2_DATA	(*(vu16*)REG_TIMER2_DATA)	//计时器2 16位值
#define TIMER3_DATA	(*(vu16*)REG_TIMER3_DATA)	//计时器3 16位值

//计时器设定 寄存器地址
//[0-1] 1 64 256 1024周期
//[2]启动级联  [6]触发中断
//[7]启动计时器
#define REG_TIMER0_CNT	(REGRAM + 0x0102)		//计时器0 设定寄存器地址
#define REG_TIMER1_CNT	(REGRAM + 0x0106)		//计时器1 设定寄存器地址
#define REG_TIMER2_CNT	(REGRAM + 0x010A)		//计时器2 设定寄存器地址
#define REG_TIMER3_CNT	(REGRAM + 0x010E)		//计时器3 设定寄存器地址
//?16/8位
#define TIMER0_CNT	(*(vu16*)REG_TIMER0_CNT)		//计时器0 设定寄存器值
#define TIMER1_CNT	(*(vu16*)REG_TIMER1_CNT)		//计时器1 设定寄存器值
#define TIMER2_CNT	(*(vu16*)REG_TIMER2_CNT)		//计时器2 设定寄存器值
#define TIMER3_CNT	(*(vu16*)REG_TIMER3_CNT)		//计时器3 设定寄存器值

//按键输入 寄存器地址
#define REG_KEY_INPUT	(REGRAM + 0x0130)
//按键输入 值
#define KEY_INPUT		(*(vu16*)REG_KEY_INPUT)
