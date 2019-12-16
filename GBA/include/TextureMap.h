#pragma once
#include <hl_math.h>
#include <hl_graphics.h>
#include "RT_base.h"


//**����Texture
//*����32bitλͼ

//λͼ��ͼ
typedef struct MAP_BITMAP
{
	GetColorFC GetColor;
	int width;		// С�� 1<<(31-FP32_FBIT)
	int height;
	rgb5 *bits;
}Map_Bitmap;

//Բ����ͼ //**�ϲ���λͼ
typedef struct MAP_GLOBE
{
	GetColorFC GetColor;
	int width;		//һ��Ŀ��
	int height;	//�����ĸ߶�
	rgb5 *bits;
}Map_Globe;

//���̸���ͼ **����Cb
typedef struct MAP_PATTERN
{
	GetColorFC GetColor;
	int number_x;
	int number_y;
}Map_Pattern;

typedef struct MAP_WAVE
{
	GetColorFC GetColor;
	fp32 size_x;
	fp32 size_y;
}Map_Wave;

Color GetColor_Bitmap(Map *map, fp32 u, fp32 v);
Color GetColor_Globe(Map *map, fp32 u, fp32 v);
Color GetColor_Pattern(Map *map, fp32 u, fp32 v);
Color GetColor_Wave(Map *map, fp32 u, fp32 v);

Map *SetMap_Bitmap(Map_Bitmap *bmp, int width, int height, rgb5 *bits);
Map *SetMap_Globe(Map_Globe *bmp, int width, int height, rgb5 *bits);
Map *SetMap_Pattern(Map_Pattern *ckb, int x, int y);
Map *SetMap_Wave(Map_Wave *wv, fp32 size_x, fp32 size_y);