#pragma once
#include <hl_gba.h>
#include <hl_system.h>

/*
typedef enum KEYCODE
{
	Key_A = 0x0001,
	Key_B = 0x0002,
	Key_Select = 0x0004,
	Key_Start = 0x0008,
	Key_Right = 0x0010,
	Key_Left = 0x0020,
	Key_Up = 0x0040,
	Key_Down = 0x0080,
	Key_R = 0x0100,
	Key_L = 0x0200
}KeyCode;*/

#define KEY_A			0
#define KEY_B			1
#define KEY_SELECT	2
#define KEY_START		3
#define KEY_RIGHT		4
#define KEY_LEFT		5
#define KEY_UP		6
#define KEY_DOWN		7
#define KEY_R1		8
#define KEY_L1		9

#define KEY_PADMASK 0b0011110000
#define KEY_KEYMASK 0b1111111111

int input_new;
int input_last;

static inline int GetNewInput()
{
	input_new = (~KEY_INPUT) & KEY_KEYMASK;
	return input_new;
}

//检测按键是否按下
static inline Bool GetKeyDown(int key)
{
	return (input_new & BIT(key)) >> key;
}

static inline int GetAxis_Horizontal()
{
	return GetKeyDown(KEY_RIGHT) - GetKeyDown(KEY_LEFT);
}

static inline int GetAxis_Vertical()
{
	return GetKeyDown(KEY_UP) - GetKeyDown(KEY_DOWN);
}

static inline int GetAxis_LR()
{
	return GetKeyDown(KEY_R1) - GetKeyDown(KEY_L1);
}

static inline int GetAxis_AB()
{
	return GetKeyDown(KEY_A) - GetKeyDown(KEY_B);
}

//检测是否有方向键按下
static inline int GetPadDown()
{
	return input_new & KEY_PADMASK;
}