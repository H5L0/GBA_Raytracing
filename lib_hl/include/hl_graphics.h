#pragma once
#include "hl_system.h"
#include "hl_gba.h"
#include "hl_color.h"


void DrawRectangle(u8 size_x, u8 size_y, u8 pos_x, u8 pos_y, rgb5 color);
void DrawLine(int start_x, int start_y, int end_x, int end_y, rgb5 color);
void ClearScreen();
void DrawAxisBackground(int x, int y);

#define MODE_0		0
#define MODE_1		1
#define MODE_2		2
#define MODE_3		3
#define MODE_4		4
#define MODE_5		5
#define FRAME_0		0
#define FRAME_1		BIT(4)
#define SPRITE_1D		BIT(6)
#define SPRITE_2D		0
#define FORCE_BLANK	BIT(7)
#define BG0_ON		BIT(8)
#define BG1_ON		BIT(9)
#define BG2_ON		BIT(10)
#define BG3_ON		BIT(11)
#define OBJ_ON		BIT(12)
#define WIN0_ON		BIT(13)
#define WIN1_ON		BIT(14)
#define OBJ_WIN_ON	BIT(15)


//设定图像模式
static inline void SetMode(int mode) { DISPCNT = mode; }

//设定马赛克大小（1-16）（1为关闭）
static inline void SetMosaic(int size)
{
	size = (size - 1) & 0b1111;
	BG2CNT |= (size == 0 ? 0 : 1 << 6);
	MOSAIC = (size << 4) + size;
}


//在Mode3下设定一像素颜色
static inline void SetPixel3(int pos_x, int pos_y, rgb5 color)
{
	*((u16 *)(VRAM + ((240 * pos_y + pos_x) << 1))) = color;
}

static inline rgb5 GetPixel3(int pos_x, int pos_y)
{
	return *((u16 *)(VRAM + ((240 * pos_y + pos_x) << 1)));
}


//**临时测试16位(10bit)颜色
static inline void SetPixel_16(int pos_x, int pos_y, Color *color)
{
	((Color *)VRAM)[240 * pos_y + pos_x] = *color;
}

static inline Color GetPixel_16(int pos_x, int pos_y)
{
	return ((Color *)VRAM)[240 * pos_y + pos_x];
}
