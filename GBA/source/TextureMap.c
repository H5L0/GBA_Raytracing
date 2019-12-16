#include "TextureMap.h"


Map *SetMap_Bitmap(Map_Bitmap *bmp, int width, int height, rgb5 *bits)
{
	bmp->GetColor = GetColor_Bitmap;
	bmp->width = width;
	bmp->height = height;
	bmp->bits = bits;
	return (Map *)bmp;
}

Color GetColor_Bitmap(Map *map, fp32 u, fp32 v)
{
	Map_Bitmap *bm = (Map_Bitmap *)map;
	//**
	if (u < 0) u = 0;
	else if (u > FP32_1) u = FP32_1;
	if (v < 0) v = 0;
	else if (v > FP32_1) v = FP32_1;

	int x = (u * (bm->width - 1)) >> FP32_FBIT;
	int y = ((FP32_1 - v) * (bm->height - 1)) >> FP32_FBIT;
	return color16_rgb5(bm->bits[y * bm->width + x]);
}


Map *SetMap_Globe(Map_Globe *bmp, int width, int height, rgb5 *bits)
{
	bmp->GetColor = GetColor_Globe;
	bmp->width = width;
	bmp->height = height / 2;
	bmp->bits = bits;
	return (Map *)bmp;
}

Color GetColor_Globe(Map *map, fp32 u, fp32 v)
{
	Map_Globe *bm = (Map_Globe *)map;
	if (u < 0) u = 0;
	else if (u > FP32_1) u = FP32_1;

	if (v < 0) v = FP32_1 * 2 - 1;
	else if (v > FP32_1 * 2 - 1) v = 0;
	else v = FP32_1 * 2 - 1 - v;
	//if (v < 0) v = 0;
	//else if (v > FP32_1 * 2 - 1) v = FP32_1 * 2 - 1;

	int x = (u * bm->width) >> FP32_FBIT;
	int y = (v * bm->height) >> FP32_FBIT;
	return color16_rgb5(bm->bits[y * bm->width + x]);
}


Map *SetMap_Pattern(Map_Pattern *ckb, int x, int y)
{
	ckb->GetColor = GetColor_Pattern;
	ckb->number_x = x;
	ckb->number_y = y;
	return (Map *)ckb;
}

Color GetColor_Pattern(Map *map, fp32 u, fp32 v)
{
	Map_Pattern *ckb = (Map_Pattern *)map;
	int pk = (((u * ckb->number_x) >> FP32_FBIT) & 1) ^ (((v * ckb->number_y) >> FP32_FBIT) & 1);
	pk = pk << FP16_FBIT;
	Color dc = { pk,pk,pk };
	/*xyz16 dc =
	{
		((hit->u * pk) >> (FP32_FBIT - FP16_FBIT)) + ((1 - pk) * 128),
		((hit->v * pk) >> (FP32_FBIT - FP16_FBIT)) + ((1 - pk) * 128),
		FP16_1 * pk + (1 - pk) * 128
	}; */
	//xyz16 dc = { fp16_fp32(hit->u), fp16_fp32(hit->v), FP16_1 };
	return dc;
}


Map *SetMap_Wave(Map_Wave *wv, fp32 size_x, fp32 size_y)
{
	wv->GetColor = GetColor_Wave;
	wv->size_x = size_x;
	wv->size_y = size_y;
	return (Map *)wv;
}

Color GetColor_Wave(Map *map, fp32 u, fp32 v)
{
	//Map_Wave *wv = (Map_Wave *)map;
	fp32 pk = (sin_ht(u * 16) + sin_ht(v * 32) + (2 << FP32_FBIT)) >> 1;
	pk = fp16_fp32(pk);
	Color dc = { pk, pk, pk };
	return dc;
}

