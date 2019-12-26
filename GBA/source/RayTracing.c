#include "RT_base.h"
#include "RT_define.h"
#include "Material.h"

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

//计算（直接）光照
Color GetLightIrradiant(Scene *sc, HitPoint *hit)
{
	//fp16 AO = GetAmbientIllumination(sc, hit);
	//Color lightResult = { AO, AO, AO };
	Color lightResult = sc->ambientLightColor;

	//遍历灯光
	Entity *ptLight;
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
//全局光照固定灯光
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
