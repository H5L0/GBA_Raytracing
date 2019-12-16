#pragma once
#include <hl_math.h>
#include <hl_graphics.h>
#include "RT_base.h"

typedef struct SCENE Scene;
typedef struct HITPOINT HitPoint;
typedef struct RAY Ray;



//ͨ�õķ������
typedef struct MATERIAL_REFLECT
{
	ShadeFC Shade;
	Color color;
	fp16 metallic;		//������
	fp16 reflectance;	//������
	fp16 roughness;		//�ֲڶ�
	Map *map_diffuse;	//��������ͼָ��
	Map *map_reflect;	//������ͼָ��
}MT_Reflect;

//�ƹ����
typedef struct MATERIAL_LIGHT
{
	ShadeFC Shade;
	Color color;
	u16 intensity;
	u32 samples;
}MT_Light;

//͸������
typedef struct MATERIAL_TRANSPARENT
{
	ShadeFC Shade;
	Color color;
	fp16 transparency;
	Map *map_alpha;		//͸������ͼָ��
}MT_Transparent;



//��ɫ����
Color Shade_Reflect(Scene *sc, HitPoint *hit, Ray *ray);
Color Shade_Refract(Scene *sc, HitPoint *hit, Ray *ray);
Color Shade_Light(Scene *sc, HitPoint *hit, Ray *ray);
Color Shade_Transparent(Scene *sc, HitPoint *hit, Ray *ray);
Color Shade_Glory(Scene *sc, HitPoint *hit, Ray *ray);

//**��Material�ŵ�������.c�ļ��У�������ɫ����

static inline MT_Reflect *SetMaterial_Reflect(MT_Reflect *mt,
	u8 metallic100, u8 reflect100, u8 rough100, Map *map_diffuse, Map *map_reflect)
{
	mt->Shade = Shade_Reflect;
	mt->metallic = fp16_100f(metallic100);
	mt->reflectance = fp16_100f(reflect100);
	mt->roughness = fp16_100f(rough100);
	mt->map_diffuse = map_diffuse;
	mt->map_reflect = map_diffuse;
	return mt;
}

static inline MT_Light *SetMaterial_Light(MT_Light *mt, u8 r100, u8 g100, u8 b100, u16 intensity)
{
	mt->Shade = Shade_Light;
	//��ʵ��color�ķ�Χ��0~31
	mt->color.r = fp16_100f(r100);
	mt->color.g = fp16_100f(g100);
	mt->color.b = fp16_100f(b100);
	//**�����˵ƹ���ɫ��ֵ**
	color_clamp01(&mt->color);
	//*����ǿ�ȣ�����
	mt->intensity = intensity;
	//***
	mt->samples = 0;
	return mt;
}

static inline MT_Transparent *SetMaterial_Transparent(MT_Transparent *mt, u8 r100, u8 g100, u8 b100, u8 trans100, Map *map_alpha)
{
	mt->Shade = Shade_Transparent;
	mt->color.r = fp16_100f(r100);
	mt->color.g = fp16_100f(g100);
	mt->color.b = fp16_100f(b100);
	mt->transparency = fp16_100f(trans100);
	mt->map_alpha = map_alpha;
	return mt;
}
