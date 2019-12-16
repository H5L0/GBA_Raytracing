#include "RT_base.h"
#include "RT_define.h"
#include "Material.h"

//因为是单线程程序，使用全局变量来减少运行时定义变量
//但需要注意嵌套函数前后文覆盖
//HitPoint hit_1;
//HitPoint hit_2;
//HitPoint hit_3;
//HitPoint *out_hit = &hit_1;	//指向击中点信息的指针
//HitPoint *ns_hit = &hit_2;		//指向最近击中点信息的指针
//HitPoint *light_hit = &hit_3;	//指向灯光检测击中点信息的指针

//**static修饰全局变量会限定在此文件范围内


Color Trace(Scene *sc, Ray *ray)
{
	if (ray->depth++ > 5) return _color_black;

	Entity *ptet;

	//***试试局部变量
	HitPoint hit_1;
	HitPoint hit_2;
	hit_1.quickCheck = FALSE;
	hit_2.quickCheck = FALSE;
	HitPoint *out_hit = &hit_1;		//指向击中点信息的指针
	HitPoint *ns_hit = &hit_2;		//指向最近击中点信息的指针

	ns_hit->distance = FP32_MAX;
	Bool haveHit = FALSE;
	//遍历物体和灯光
	//#pragma loop(hint_parallel(8))
	for (int i = 0; i < sc->totalCount; i++)
	{
		ptet = &sc->entities[i];
		//此物体是否已开启且可见
		if (NOFLAG(ptet->property, EP_Enable)) continue;
		if (NOFLAG(ptet->property, EP_Visable)) continue;
		//通过指针调用光线投射函数
		if (ptet->ptShape->fc->Raycast(ptet, ray, out_hit))
		{
			if (out_hit->distance < ns_hit->distance)
			{
				HitPoint *temp = ns_hit;
				ns_hit = out_hit;
				out_hit = temp;
				haveHit = TRUE;
			}
		}
	}

	if (haveHit) return (*(ns_hit->et->ptMaterial->Shade))(sc, ns_hit, ray);
	else return sc->GetSkyColor(&ray->direction);
}


Color _GetLightIrradiant(Scene *sc, HitPoint *hit)
{
	EntityProperty et_property = hit->et->property;
	Entity *ptLight, *ptEntity;
	Ray lray;
	OffsetHitPoint(&lray.start, hit);
	HitPoint hit_4;
	HitPoint *light_hit = &hit_4;
	light_hit->quickCheck = TRUE;

	//fp16 AO = GetAmbientIllumination(sc, hit)>>1;
	//Color lightResult = { AO, AO, AO };
	Color lightResult = sc->ambientLightColor;
	fp32 distance2;

	//遍历灯光
	for (int i = sc->objectCount; i < sc->totalCount; i++)
	{
		ptLight = &sc->entities[i];
		if (NOFLAG(ptLight->property, EP_Enable)) continue;

		distance2 = xyz_len2(xyz_sub(&lray.direction, &ptLight->transform->position, &lray.start));
		xyz_normalize(&lray.direction);

		//着色物体接受阴影
		if (HASFLAG(et_property, EP_ReceiveShadows))
		{
			//遍历物体
			for (int k = 0; k < sc->objectCount; k++)
			{
				ptEntity = &sc->entities[k];

				//跳过物体自身阴影
				if (ptEntity == hit->et && NOFLAG(et_property, EP_ReceiveSelfShadow)) continue;

				//此物体投射阴影
				if (HASFLAGS(ptEntity->property, EP_Visable | EP_CastShadows))
				{
					//对物体射线检测。ns_hit正在使用，out_hit已空出，但不安全。因此另外使用light_hit
					if ((*(ptEntity->ptShape->fc->Raycast))(ptEntity, &lray, light_hit))
					{
						//如果碰撞到物体，不计算光照，跳转到下一个灯光
						goto nextLight;
					}
				}
			}
		}

		//增加此灯光照
		//sr:fp32
		s64 sr = xyz_dot(&lray.direction, &hit->normal);
		if (sr <= 0) continue;

		MT_Light *mt = (MT_Light *)ptLight->ptMaterial;
		//sr:fp32 光照强度
		sr = ((sr * mt->intensity) << FP32_FBIT) / distance2;
		if (sr > FP32_1 * 8) sr = FP32_1 * 8;

		lightResult.r += (fp16)((sr * mt->color.r) >> FP32_FBIT);
		lightResult.g += (fp16)((sr * mt->color.g) >> FP32_FBIT);
		lightResult.b += (fp16)((sr * mt->color.b) >> FP32_FBIT);

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

		nextLight:;
	}

	return lightResult;
}

Color GetLightIrradiant(Scene *sc, HitPoint *hit)
{
	//fp16 AO = GetAmbientIllumination(sc, hit)>>1;
	//Color lightResult = { AO, AO, AO };
	Color lightResult = sc->ambientLightColor;

	//遍历灯光
	Entity *ptLight;
	//#pragma loop(hint_parallel(8))
	for (int i = sc->objectCount; i < sc->totalCount; i++)
	{
		ptLight = &sc->entities[i];
		if (NOFLAG(ptLight->property, EP_Enable)) continue;

		Color result = ptLight->ptShape->fc->light(sc, ptLight, hit);

		color_add(&lightResult, &lightResult, &result);
	}
	return lightResult;
}

/*
Vector3 SkyLightDirections[160];
int skyLightAmount = 0;
Vector3 *GetSkyLights()
{
	int hsub = 11;
	fp32 y, r, l;
	fp32 d = FP32_1 / (hsub * 2);
	for (int i = 1; i < hsub; i++)
	{
		l = i * FP32_1 / 3;
		y = sin_ht(l);
		r = cos_ht(l);
		int pt = 2 * r / d;
		if (pt == 0) continue;
		fp32 dh = (4 * FP32_1) / pt;
		for (int k = 0; k < pt; k++)
		{
			SkyLightDirections[skyLightAmount].x = sin_ht(k * dh);
			SkyLightDirections[skyLightAmount].y = y;
			SkyLightDirections[skyLightAmount].z = cos_ht(k * dh);
			skyLightAmount++;
		}
	}
	return 0;
}

fp16 GetAmbientIllumination(Scene *sc, HitPoint *hit)
{
	//着色物体不接受阴影
	if (NOFLAG(hit->et->property, EP_ReceiveShadows)) return FP16_1;

	Entity *ptEntity;
	Ray lray;
	fp16 result = 0;
	fp16 ipl = FP16_1 / skyLightAmount;
	lray.start.x = hit->position.x + (hit->normal.x >> 8);
	lray.start.y = hit->position.y + (hit->normal.y >> 8);
	lray.start.z = hit->position.z + (hit->normal.z >> 8);
	light_hit->quickCheck = TRUE;

	for (int i = 0; i < skyLightAmount; i++)
	{
		lray.direction = SkyLightDirections[i];
		//遍历物体
		for (int k = 0; k < sc->objectCount; k++)
		{
			ptEntity = &sc->entities[k];
			//此物体投射阴影
			if (NOFLAG(hit->et->property, EP_CastShadows)) continue;
			//对物体射线检测
			if ((*(ptEntity->ptShape->fc->Raycast))(ptEntity, &lray, light_hit))
			{
				//如果碰撞到物体，不计算光照，跳转到下一个灯光
				goto nextPoint;
			}
		}
		result += ipl;
		nextPoint:;
	}
	return result;
}
*/
