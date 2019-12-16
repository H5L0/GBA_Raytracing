#include "RT_base.h"
#include "Shape.h"
#include "Material.h"

//static HitPoint hit_4;
//static HitPoint *lightHit = &hit_4;

static inline Bool LightRayCheck(Scene *sc, HitPoint *hit, Ray *lray, EntityProperty et_property, HitPoint *lightHit)
{
	//着色物体接受阴影
	if (NOFLAG(et_property, EP_ReceiveShadows)) return TRUE;

	//遍历物体
	for (int k = 0; k < sc->objectCount; k++)
	{
		Entity *ptEntity = &sc->entities[k];

		//自身不接受自己的阴影
		if (ptEntity == hit->et && NOFLAG(et_property, EP_ReceiveSelfShadow)) continue;

		//此物体可见且投射阴影
		if (HASFLAGS(ptEntity->property, EP_Visable | EP_CastShadows))
		{
			//对物体射线检测。ns_hit正在使用，out_hit已空出，但不安全。因此另外使用light_hit
			if (ptEntity->ptShape->fc->Raycast(ptEntity, lray, lightHit))
			{
				//如果碰撞到物体，不计算光照，跳转到下一次采样
				return FALSE;
			}
		}
	}
	return TRUE;
}

Color Lit_Sphere(Scene *sc, Entity *ptLight, HitPoint *hit)
{
	Color result = { 0, 0, 0 };
	EntityProperty et_property = hit->et->property;
	MT_Light *mt = (MT_Light *)ptLight->ptMaterial;
	Sphere *sp = (Sphere *)ptLight->ptShape;

	HitPoint hit_4;
	HitPoint *lightHit = &hit_4;

	Ray lray;
	OffsetHitPoint(&lray.start, hit);
	lightHit->quickCheck = TRUE;

	Vector3 *ltPosition = &ptLight->transform->position;
	fp32 distance2;
	Vector3 temp;
	//Entity *ptEntity;

	//硬阴影
	if (mt->samples == 1)
	{
		distance2 = xyz_len2(xyz_sub(&lray.direction, ltPosition, &lray.start));
		xyz_normalize(&lray.direction);

		s64 sr = xyz_dot(&lray.direction, &hit->normal);
		if (sr <= 0) return result;

		if (LightRayCheck(sc, hit, &lray, et_property, lightHit))
		{
			//sr:fp32 光照强度
			sr = ((sr * mt->intensity) << FP32_FBIT) / distance2;
			if (sr > FP32_1 * 8) sr = FP32_1 * 8;

			result.r = (fp16)((sr * mt->color.r) >> FP32_FBIT);
			result.g = (fp16)((sr * mt->color.g) >> FP32_FBIT);
			result.b = (fp16)((sr * mt->color.b) >> FP32_FBIT);
		}
		return result;
	}

	//8采样软阴影
	for (int s = 0; s < 8; s++)
	{
		//0.577 -> 605396
		temp.x = ltPosition->x + (((s & 0b001) == 0 ? -1 : 1) * sp->radius);
		temp.y = ltPosition->y + (((s & 0b010) == 0 ? -1 : 1) * sp->radius);
		temp.z = ltPosition->z + (((s & 0b100) == 0 ? -1 : 1) * sp->radius);

		distance2 = xyz_len2(xyz_sub(&lray.direction, &temp, &lray.start));
		xyz_normalize(&lray.direction);

		fp32 cos = xyz_dot(&lray.direction, &hit->normal);
		if (cos <= 0) continue;

		if (LightRayCheck(sc, hit, &lray, et_property, lightHit))
		{
			//增加此灯光照
			//sr:fp32 光照强度
			s64 sr = (((s64)cos * mt->intensity) << FP32_FBIT) / distance2;
			//if (sr > FP32_1 * 8) sr = FP32_1 * 8;

			sr = sr >> 3;
			result.r += (fp16)((sr * mt->color.r) >> FP32_FBIT);
			result.g += (fp16)((sr * mt->color.g) >> FP32_FBIT);
			result.b += (fp16)((sr * mt->color.b) >> FP32_FBIT);

			//sr = fp32_div64(sr * mt->intensity, distance2);
			//fp32 * fp16
			//lightResult.r += fp32_mul64(sr, mt->color.r);
			//lightResult.g += fp32_mul64(sr, mt->color.g);
			//lightResult.b += fp32_mul64(sr, mt->color.b);

			// 31bit + 27bit - 31bit - 10bit = 17bit
			// [r]bit + 27bit - [d]bit -10bit = [0,36]bit
			//sr = ((((u64)sr) << FP32_FBIT ) / distance2) >> (FP32_FBIT - FP16_FBIT);
			//if (sr > ((1 << 17) - 1)) sr = (1 << 17) - 1;
			//14bit + 17bit = 31bit
			//lightResult.x += ((color->x >> 2) * sr) >> 15;
			//lightResult.y += ((color->y >> 2) * sr) >> 15;
			//lightResult.z += ((color->z >> 2) * sr) >> 15;
		}
	}

	return result;
}

Color Lit_Plane(Scene *sc, Entity *ptLight, HitPoint *hit)
{
	return _color_black;
}

Color Lit_Box(Scene *sc, Entity *ptLight, HitPoint *hit)
{
	HitPoint hit_4;
	HitPoint *lightHit = &hit_4;

	Color result = { 0, 0, 0 };

	Ray lray;
	//Entity *ptEntity;
	EntityProperty et_property = hit->et->property;

	xyz_neg(&lray.direction, &ptLight->transform->rotation.forward);
	OffsetHitPoint(&lray.start, hit);
	lightHit->quickCheck = TRUE;

	if (LightRayCheck(sc, hit, &lray, et_property, lightHit))
	{
		//增加此灯光照
		//sr:fp32
		s64 sr = xyz_dot(&lray.direction, &hit->normal);
		if (sr <= 0) return result;

		//sr:fp32 光照强度
		//sr = ((sr * mt->intensity) << FP32_FBIT);
		//if (sr > FP32_1 * 8) sr = FP32_1 * 8;

		MT_Light *mt = (MT_Light *)ptLight->ptMaterial;
		result.r = (fp16)((sr * mt->color.r) >> FP32_FBIT);
		result.g = (fp16)((sr * mt->color.g) >> FP32_FBIT);
		result.b = (fp16)((sr * mt->color.b) >> FP32_FBIT);
	}

	return result;
}

Color Lit_Triangle(Scene *sc, Entity *ptLight, HitPoint *hit)
{
	return _color_black;
}

Color Lit_Mesh(Scene *sc, Entity *ptLight, HitPoint *hit)
{
	return _color_black;
}