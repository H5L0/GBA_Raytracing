#include "RT_base.h"
#include "RT_define.h"
#include "Material.h"


Color Shade_Depth(Scene *sc, HitPoint *hit, Ray *ray)
{
	int c = (FP32_1 * 31) / (((FP32_1 * 6) + hit->distance) >> 8);
	Color color = { c, c, c };
	return color;
}


//ͨ�÷��������ɫ
Color Shade_Reflect(Scene *sc, HitPoint *hit, Ray *ray)
{
	//return Shade_Depth(sc, hit, ray);
	MT_Reflect *mt = (MT_Reflect *)hit->et->ptMaterial;
	//��������ɫ
	Color diffuseColor;
	if (mt->map_diffuse == NULL) diffuseColor = mt->color;
	else diffuseColor = mt->map_diffuse->GetColor(mt->map_diffuse, hit->u, hit->v);
	//������������ɫ�ܹ���Ӱ��
	Color lightColor = GetLightIrradiant(sc, hit);
	//�������ɫ������������ɫ
	color_mul(&lightColor, &lightColor, &diffuseColor);
	//��ȫ������ �� �������ݹ� (�� n = 3 ����4��)
	if (mt->reflectance == 0 || ray->depth > 3) return lightColor;

	//���淴��
	//Ray rf_ray;
	//ray->start = hit->position;
	OffsetHitPoint(&ray->start, hit);
	xyz_reflect(&ray->direction, &ray->direction, &hit->normal);
	Color reflectColor = Trace(sc, ray);
	Color finalColor;
	//�����䵽��ȫ���淴��Ĳ�ֵ
	//(light * color) -> reflect
	//xyz16_lerp(&finalColor, &lightColor, &reflectColor, mt->reflectance);
	//�����䵽��ɫ����Ĳ�ֵ
	//light * color -> reflect * color
	//finalColor.x = fp16_mul32(FP16_1 - mt->reflectance, lightColor.x) + fp16_mul32(mt->reflectance, fp16_mul32(mt->color.x, reflectColor.x));
	//finalColor.y = fp16_mul32(FP16_1 - mt->reflectance, lightColor.y) + fp16_mul32(mt->reflectance, fp16_mul32(mt->color.y, reflectColor.y));
	//finalColor.z = fp16_mul32(FP16_1 - mt->reflectance, lightColor.z) + fp16_mul32(mt->reflectance, fp16_mul32(mt->color.z, reflectColor.z));

	//(light * color) -> (reflect * (1 -> color))
	color_mul(&finalColor, &reflectColor, &diffuseColor);
	color_lerp(&finalColor, &reflectColor, &finalColor, mt->metallic);
	color_lerp(&finalColor, &lightColor, &finalColor, mt->reflectance);

	return finalColor;	//�ݹ���ֹ��ɫ
}

//���������ɫ
Color Shade_Refract(Scene *sc, HitPoint *hit, Ray *ray)
{
	MT_Reflect *mt = (MT_Reflect *)hit->et->ptMaterial;
	//fp32 rft = FP32_1;
	OffsetHitPoint_Reverse(&ray->start, hit);
	if (xyz_dot16(&hit->normal, &ray->direction) > 0)
	{
		xyz32_refract(&ray->direction, &ray->direction, &hit->normal, FP32_1 + FP32_H5, FP32_1);
	}
	else
	{
		xyz32_refract(&ray->direction, &ray->direction, &hit->normal, FP32_1, FP32_1 + FP32_H5);
	}

	Color refractColor = Trace(sc, ray);
	color_lerp(&refractColor, &refractColor, &mt->color, FP16_1 >> 3);
	return refractColor;
}

//�ƹ������ɫ
Color Shade_Light(Scene *sc, HitPoint *hit, Ray *ray)
{
	MT_Light *mt = (MT_Light *)hit->et->ptMaterial;
	fp16 it = mt->intensity > FP16_MAXINT ? FP16_MAXINT : mt->intensity;
	Color color = { mt->color.r * it, mt->color.g * it, mt->color.b * it };
	return color;
}

//͸��������ɫ
Color Shade_Transparent(Scene *sc, HitPoint *hit, Ray *ray)
{
	MT_Transparent *mt = (MT_Transparent *)hit->et->ptMaterial;

	Color lightColor = GetLightIrradiant(sc, hit);
	color_mul(&lightColor, &lightColor, &mt->color);

	fp16 tp;
	if (mt->map_alpha == NULL) tp = mt->transparency;
	else
	{
		Color alpha = mt->map_alpha->GetColor(mt->map_alpha, hit->u, hit->v);
		//**��ʱ����alpha
		tp = FP16_1 - alpha.r;
		tp = FP16_1 - fp16_mul32(tp, tp);
	}

	if (tp == FP16_1) return lightColor;

	//Ϊ��ʼ�����ƫ�ƣ���������
	fp16 dot = xyz_dot16(&ray->direction, &hit->normal);
	if (dot < 0) OffsetHitPoint_Reverse(&ray->start, hit);
	else OffsetHitPoint(&ray->start, hit);

	Color result = Trace(sc, ray);
	color_lerp(&result, &result, &lightColor, tp);
	return result;
}



Color Shade_Glory(Scene *sc, HitPoint *hit, Ray *ray)
{
	MT_Transparent *mt = (MT_Transparent *)hit->et->ptMaterial;
	fp16 dot = xyz_dot16(&ray->direction, &hit->normal);

	//Ϊ��ʼ�����ƫ�ƣ���������
	if (dot < 0)
	{
		OffsetHitPoint_Reverse(&ray->start, hit);
		dot = FP16_1 + dot;
	}
	else
	{
		OffsetHitPoint(&ray->start, hit);
		dot = FP16_1 - dot;
	}

	fp16 d = 640;
	if (dot > d) dot = fp16_div32(FP16_1 - dot, FP16_1 - d);
	else dot = fp16_div32(dot, d);
	//dot = fp16_mul32(dot, dot);
	dot = fp16_mul32(dot, fp16_mul32(dot, dot));

	Color light = GetLightIrradiant(sc, hit);
	color_mul(&light, &light, &mt->color);

	Color result = Trace(sc, ray);
	color_lerp(&result, &result, &light, fp16_mul32(dot, mt->transparency));
	return result;
}
