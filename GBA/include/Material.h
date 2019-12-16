#pragma once
#include <hl_math.h>
#include <hl_graphics.h>
#include "RT_base.h"

typedef struct SCENE Scene;
typedef struct HITPOINT HitPoint;
typedef struct RAY Ray;



//通用的反射材质
typedef struct MATERIAL_REFLECT
{
	ShadeFC Shade;
	Color color;
	fp16 metallic;		//金属度
	fp16 reflectance;	//反射率
	fp16 roughness;		//粗糙度
	Map *map_diffuse;	//漫反射贴图指针
	Map *map_reflect;	//反射贴图指针
}MT_Reflect;

//灯光材质
typedef struct MATERIAL_LIGHT
{
	ShadeFC Shade;
	Color color;
	u16 intensity;
	u32 samples;
}MT_Light;

//透明材质
typedef struct MATERIAL_TRANSPARENT
{
	ShadeFC Shade;
	Color color;
	fp16 transparency;
	Map *map_alpha;		//透明度贴图指针
}MT_Transparent;



//着色函数
Color Shade_Reflect(Scene *sc, HitPoint *hit, Ray *ray);
Color Shade_Refract(Scene *sc, HitPoint *hit, Ray *ray);
Color Shade_Light(Scene *sc, HitPoint *hit, Ray *ray);
Color Shade_Transparent(Scene *sc, HitPoint *hit, Ray *ray);
Color Shade_Glory(Scene *sc, HitPoint *hit, Ray *ray);

//**把Material放到单独的.c文件中，包括着色函数

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
	//事实上color的范围是0~31
	mt->color.r = fp16_100f(r100);
	mt->color.g = fp16_100f(g100);
	mt->color.b = fp16_100f(b100);
	//**限制了灯光颜色的值**
	color_clamp01(&mt->color);
	//*光照强度，整数
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
