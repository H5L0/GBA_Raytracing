#pragma once
#include <hl_math.h>

typedef unsigned short	rgb5;			//16位颜色，每通道5位域
typedef unsigned int		rgb8;			//32位颜色，每通道8位域
typedef unsigned int		rgb10;			//32位颜色，每通道10位域

/*typedef union
{
	struct { short r : 5, g : 5, b : 5; };
	short value;
}_rgb5;*/

//16位颜色  r,g,b -> [0,32)
#define RGB5(r,g,b)		(((b)<<10)|((g)<<5)|(r))
//32位颜色  r,g,b -> [0,256)
#define RGB8(r,g,b)		(((b)<<16)|((g)<<8)|(r))
//32位颜色  r,g,b -> [0,1024)
#define RGB10(r,g,b)		(((b)<<20)|((g)<<10)|(r))

#define RGB5_BLACK	0
#define RGB5_RED		RGB5(31, 0, 0)
#define RGB5_GREEN	RGB5(0, 31, 0)
#define RGB5_BLUE		RGB5(0, 0, 31)
#define RGB5_WHITE	RGB5(31,31,31)

#define RGB10_RED		RGB10(1023, 0, 0)
#define RGB10_GREEN	RGB10(0, 1023, 0)
#define RGB10_BLUE	RGB10(0, 0, 1023)
#define RGB10_WHITE	RGB10(1023, 1023, 1023)

/*----------------------------Color函数声明------------------------------*/

static inline Color *color_set(Color *vc, fp16 r, fp16 g, fp16 b);
static inline Color *color_setint(Color *vc, int r, int g, int b);
static inline Color *color_add(Color *vc, Color *a, Color *b);
static inline Color *color_sub(Color *vc, Color *a, Color *b);
static inline Color *color_scale(Color *vc, Color *a, fp16 n);
static inline Color *color_mul(Color *vc, Color *a, Color *b);
static inline Color *color_lerp(Color *vc, Color *a, Color *b, fp16 t);
static inline Color *color_clamp01(Color *vc);

//---------------------------不同颜色格式转换--------------------------//

static inline rgb5 rgb10_rgb5(rgb10 color);
static inline Color color16_rgb5(rgb5 color);
static inline rgb5 rgb5_color16(Color *vc);
//static inline rgb5 rgb5_xyz32(xyz32 *vc);
//inline?
rgb5 rgb5_Lerp(rgb5 a, rgb5 b, fp32 t);
rgb10 rgb10_Lerp(rgb10 a, rgb10 b, fp16 t);



/*-----------------------------Color函数定义-------------------------------*/
//设定xyz16的值
static inline Color *color_set(Color *vc, fp16 r, fp16 g, fp16 b)
{ vc->r = r; vc->g = g; vc->b = b; return vc; }
//使用int设置矢量
static inline Color *color_setint(Color *vc, int r, int g, int b)
{ vc->r = fp16_int(r); vc->g = fp16_int(g); vc->b = fp16_int(b); return vc; }
//vc = a + b
static inline Color *color_add(Color *vc, Color *c1, Color *c2)
{ vc->r = c1->r + c2->r; vc->g = c1->g + c2->g; vc->b = c1->b + c2->b; return vc; }
//vc = a - b
static inline Color *color_sub(Color *vc, Color *c1, Color *c2)
{ vc->r = c1->r - c2->r; vc->g = c1->g - c2->g; vc->b = c1->b - c2->b; return vc; }
//vc = a * n
static inline Color *color_scale(Color *vc, Color *c, fp16 n)
{
	vc->r = fp16_mul32(c->r, n);
	vc->g = fp16_mul32(c->g, n);
	vc->b = fp16_mul32(c->b, n);
	return vc;
}
//vc = a·b （点乘）
static inline Color *color_mul(Color *vc, Color *c1, Color *c2)
{
	vc->r = fp16_mul32(c1->r, c2->r);
	vc->g = fp16_mul32(c1->g, c2->g);
	vc->b = fp16_mul32(c1->b, c2->b);
	return vc;
}
//vc = a -> b 插值
static inline Color *color_lerp(Color *vc, Color *c1, Color *c2, fp16 t)
{
	vc->r = c1->r + fp16_mul32(c2->r - c1->r, t);
	vc->g = c1->g + fp16_mul32(c2->g - c1->g, t);
	vc->b = c1->b + fp16_mul32(c2->b - c1->b, t);
	return vc;
}
//将xyz16钳制到 0~1 (不包括1)
static inline Color *color_clamp01(Color *vc)
{
	vc->r = vc->r < FP16_1 ? (vc->r < 0 ? 0 : vc->r) : (FP16_1 - 1);
	vc->g = vc->g < FP16_1 ? (vc->g < 0 ? 0 : vc->g) : (FP16_1 - 1);
	vc->b = vc->b < FP16_1 ? (vc->b < 0 ? 0 : vc->b) : (FP16_1 - 1);
	return vc;
}


//---------------------------不同颜色格式转换--------------------------//

static inline rgb5 rgb10_rgb5(rgb10 color)
{
	return ((color & RGB10_BLUE) >> 15) + ((color & RGB10_GREEN) >> 10) + ((color & RGB10_RED) >> 5);
}

//将Color转换为xyz16
static inline Color color16_rgb5(rgb5 color)
{
	Color vc = {
		(color & 0b11111) << (FP16_FBIT - 5),
		((color >> 5) & 0b11111) << (FP16_FBIT - 5),
		((color >> 10) & 0b11111) << (FP16_FBIT - 5) };
	return vc;
}

//将xyz16转化为Color (0-31)
static inline rgb5 rgb5_color16(Color *vc)
{
	return ((vc->r < 0 ? 0 : vc->r < FP16_1 ? vc->r : (FP16_1 - 1)) >> (FP16_FBIT - 5))
		+ (((vc->g < 0 ? 0 : vc->g < FP16_1 ? vc->g : (FP16_1 - 1)) >> (FP16_FBIT - 5)) << 5)
		+ (((vc->b < 0 ? 0 : vc->b < FP16_1 ? vc->b : (FP16_1 - 1)) >> (FP16_FBIT - 5)) << 10);
}

/*//将xyz32转化为Color (0-31)
static inline rgb5 rgb5_xyz32(xyz32 *vc)
{
	return ((vc->x < 0 ? 0 : vc->x < FP32_1 ? vc->x : (FP32_1 - 1)) >> (FP32_FBIT - 5))
		+ (((vc->y < 0 ? 0 : vc->y < FP32_1 ? vc->y : (FP32_1 - 1)) >> (FP32_FBIT - 5)) << 5)
		+ (((vc->z < 0 ? 0 : vc->z < FP32_1 ? vc->z : (FP32_1 - 1)) >> (FP32_FBIT - 5)) << 10);
}*/

/*
static inline _rgb5 _rgb5_test()
{
	_rgb5 color = { 16, 31, 16 };
	color.r = color.g;
	short color_s = color.value;
	return color;
}
*/