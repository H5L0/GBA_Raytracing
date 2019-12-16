#include "RT_base.h"
#include "Shape.h"
#include "Transform.h"

//按圆球进行UV映射
static inline void UV_Sphere(HitPoint *hit)
{
	//hit->u = (hit->normal.x + FP32_1) >> 1;
	//hit->v = (hit->normal.y + FP32_1) >> 1;
	fp32 dlen = FP32_1;
	if (hit->normal.y < 0) dlen -= asin_ht(-hit->normal.y);
	else dlen -= asin_ht(hit->normal.y);
	//****x&&z==0的情况需要解决
	//fp32 ls = fp32_div64(dlen, len_xy(hit->normal.x, hit->normal.z));
	fp32 ls = FP32_1;
	//fp32 ls = FP32_1 - asin_ht(hit->normal.y > 0 ? hit->normal.y : -hit->normal.y);
	hit->u = (FP32_1 + fp32_mul64(hit->normal.x, ls)) >> 1;
	hit->v = (FP32_1 + fp32_mul64(hit->normal.z, ls)) >> 1;
	if (hit->normal.y > 0) hit->v += FP32_1;
	//**v反了
}

//按圆柱体进行UV映射
static inline void UV_Cylinder(HitPoint *hit)
{
	hit->u = 0;
	//hit->v = (hit->normal.y + FP32_1) >> 1;
	hit->v = (FP32_1 + asin_ht(hit->normal.y)) >> 1;
}


//对椭圆进行光线投射检测
//返回0/1：未击中/击中   point：击中点位置和法线   distance：击中点到出射点距离
int Raycast_Sphere(Entity *et, Ray *ray, HitPoint *hit)
{
	//球半径r
	fp32 r = ((Sphere *)et->ptShape)->radius;
	fp32 r2 = fp32_pow2(r);

	//出射点P 到球心O
	Vector3 SP;
	xyz_sub(&SP, &et->transform->position, &ray->start);

	s64 sp2_64 = xyz_len2_64(&SP);
	if (sp2_64 > FP32_MAX) return FALSE;
	//入射点到球心距离平方 sp2
	fp32 sp2 = sp2_64;

	//出射点P 到射线上距球心最近点D 的长度
	fp32 pd = xyz_dot(&SP, &ray->direction);
	//如果在圆内可能为负值

	//球心O 到最近点D 的长度平方 (od2 = sp2 - pd2)
	fp32 od2 = sp2 - fp32_pow2(pd);

	//入射点在球内，必有交点
	if (sp2 < r2)
	{
		hit->distance = pd + fp32_sqrt64(r2 - od2);
		//r = -r;	//使内部法线反向
	}
	else
	{
		//射线与圆心距离大于半径，返回
		if (od2 > r2) return FALSE;

		//交点不在射线方向，返回
		if (pd < 0) return FALSE;

		hit->distance = pd - fp32_sqrt64(r2 - od2);
	}

	//快速检测返回（需要包含距离）
	if (hit->quickCheck) return TRUE;

	hit->et = et;
	ray_point(&hit->position, ray, hit->distance);
	//设定击中点法线
	xyz_sub(&hit->normal, &hit->position, &et->transform->position);
	xyz_scale_ms(&hit->normal, fp32_div64(FP32_1, r));

	UV_Sphere(hit);
	return TRUE;
}


int Raycast_Sphere_V(Entity *et, Ray *ray, HitPoint *hit)
{
	if (Raycast_Sphere(et, ray, hit))
	{
		UV_Cylinder(hit);
		return TRUE;
	}
	return FALSE;
}


//对圆柱体进行光线投射检测 **从上面复制的，未工作
//返回0/1：未击中/击中   point：击中点位置和法线   distance：击中点到出射点距离
int Raycast_Cylinder(Entity *et, Ray *ray, HitPoint *hit)
{
	//球半径r
	fp32 r = ((Sphere *)et->ptShape)->radius;
	fp32 r2 = fp32_pow2(r);

	//出射点P 到中心O
	Vector3 SP;
	xyz_sub(&SP, &et->transform->position, &ray->start);
	//入射点到球心距离平方 sp2
	fp32 sp2 = xyz_len2(&SP);

	//出射点P 到射线上距球心最近点D 的长度
	fp32 pd = xyz_dot(&SP, &ray->direction);
	//如果在圆内可能为负值

	//球心O 到最近点D 的长度平方 (od2 = sp2 - pd2)
	fp32 od2 = sp2 - fp32_pow2(pd);

	//入射点在球内，必有交点
	if (sp2 < r2)
	{
		hit->distance = pd + fp32_sqrt64(r2 - od2);
		//r = -r;	//使内部法线反向
	}
	else
	{
		//射线与圆心距离大于半径，返回
		if (od2 > r2) return FALSE;

		//交点不在射线方向，返回
		if (pd < 0) return FALSE;

		hit->distance = pd - fp32_sqrt64(r2 - od2);
	}

	//快速检测返回（需要包含距离）
	if (hit->quickCheck) return TRUE;

	hit->et = et;
	ray_point(&hit->position, ray, hit->distance);
	//设定击中点法线
	xyz_sub(&hit->normal, &hit->position, &et->transform->position);
	xyz_scale_ms(&hit->normal, fp32_div64(FP32_1, r));

	//hit->u = (hit->normal.x + FP32_1) >> 1;
	//hit->v = (hit->normal.y + FP32_1) >> 1;
	fp32 dlen = FP32_1;
	if (hit->normal.y < 0) dlen -= asin_ht(-hit->normal.y);
	else dlen -= asin_ht(hit->normal.y);
	fp32 ls = fp32_div64(dlen, len_xy(hit->normal.x, hit->normal.z));
	hit->u = (FP32_1 + fp32_mul64(hit->normal.x, ls)) >> 1;
	hit->v = (FP32_1 + fp32_mul64(hit->normal.z, ls)) >> 1;
	return TRUE;
}


#if 1
int Raycast_Plane(Entity *et, Ray *ray, HitPoint *hit)
{
	Transform *tf = et->transform;
	Vector3 *ptup = &tf->rotation.up;

	fp32 ddot = xyz_dot(&ray->direction, ptup);
	//不向平面，跳过
	if (ddot > -FP32_F2(10)) return FALSE;

	Vector3 SP;
	fp32 hdot = xyz_dot(xyz_sub(&SP, &tf->position, &ray->start), ptup);
	//在平面下，跳过
	if (hdot > 0) return FALSE;

	//if (-hdot > FP32_F2(2) && -ddot < FP32_F2(5)) return FALSE;
	hit->distance = fp32_div64(hdot, ddot);
	//快速检测（需要包含距离）
	//if (hit->quickCheck) return TRUE;

	ray_point(&hit->position, ray, hit->distance);

	fp32 wh, hdis;// xdis, zdis;
	xyz_sub(&SP, &hit->position, &tf->position);

	hdis = xyz_dot(&SP, &tf->rotation.right);
	if ((wh = ((Plane *)et->ptShape)->x >> 1) > 0)
	{
		if (hdis > wh || hdis < -wh) return FALSE;
		hit->u = FP32_H5 + fp32_div32(hdis, wh << 1);
	}
	else hit->u = (FP32_H5 + fp32_div32(hdis, -wh << 1)) & (FP32_1 - 1);

	hdis = xyz_dot(&SP, &tf->rotation.forward);
	if ((wh = ((Plane *)et->ptShape)->z >> 1) > 0)
	{
		if (hdis > wh || hdis < -wh) return FALSE;
		hit->v = FP32_H5 + fp32_div32(hdis, wh << 1);
	}
	else hit->v = (FP32_H5 + fp32_div32(hdis, -wh << 1)) & (FP32_1 - 1);

	hit->et = et;
	hit->normal = tf->rotation.up;
	return TRUE;
	/*//是否在范围内
	fp32 offset = point->start.x - et->position->x;
	if (hdis > wh || hdis < -wh) return 0;
	wh = ((Plane *)et->ptShape)->z >> 1;
	hdis = point->start.z - et->position->z;
	if (hdis > wh || hdis < -wh) return 0;*/
}
#else
int Raycast_Plane(Entity *et, Ray *ray, HitPoint *hit)
{
	Vector3 *D, *J, *K;
	xyz32 SP, DK, SJ;
	Vector3 w, h;

	D = &ray->direction;
	J = &et->rotation->right;
	K = &et->rotation->forward;
	w = ((Plane *)et->ptShape)->x >> 1;
	h = ((Plane *)et->ptShape)->z >> 1;

	xyz_cross(&DK, D, K);

	fp32 det = xyz_dot(&DK, J);
	if (det < (1 << 5) && det>-(1 << 5)) return 0;

	xyz_sub(&SP, &ray->start, et->position);

	//后处理det?
	hit->u = fp32_div64(xyz_dot(&DK, &SP), det);
	if (w > 0 && (hit->u > w || hit->u < -w)) return 0;

	xyz_cross(&SJ, &SP, J);
	hit->v = fp32_div64(xyz_dot(&SJ, D), det);
	if (h > 0 && (hit->v > h || hit->v < -h)) return 0;

	hit->distance = fp32_div64(xyz_dot(&SJ, K), det);
	if (hit->distance < 0) return 0;

	ray_point(&hit->position, ray, hit->distance);
	if (det > 0) xyz_neg(&hit->normal, &et->rotation->up);
	else hit->normal = et->rotation->up;
	hit->u = (FP32_1 + fp32_div32(hit->u, w)) >> 1;
	hit->v = (FP32_1 + fp32_div32(hit->v, h)) >> 1;
	hit->et = et;
	return 1;
}
#endif

int Raycast_Ring(Entity *et, Ray *ray, HitPoint *hit)
{
	Vector3 *ptup = &et->transform->rotation.up;
	Vector3 *position = &et->transform->position;

	fp32 ddot = xyz_dot(&ray->direction, ptup);
	//不向平面，跳过
	if (ddot > -(1 << 10)) return FALSE;

	Vector3 SP;
	fp32 hdot = xyz_dot(xyz_sub(&SP, position, &ray->start), ptup);
	//在平面下，跳过
	if (hdot > 0) return FALSE;

	hit->distance = fp32_div64(hdot, ddot);
	ray_point(&hit->position, ray, hit->distance);

	xyz_sub(&SP, &hit->position, position);
	Plane *pl = (Plane *)et->ptShape;
	fp32 r = xyz_len(&SP);
	//不在圆环内返回
	if (r < pl->x || r > pl->z) return FALSE;
	hit->u = fp32_div64(r - pl->x, pl->z - pl->x);
	hit->v = FP32_1;
	hit->et = et;
	hit->normal = *ptup;
	return TRUE;
}

#if 1
int Raycast_Box(Entity *et, Ray *ray, HitPoint *hit)
{
	Vector3 *ptRotation = &et->transform->rotation.vectors[0];
	Vector3 *ptNormal;
	fp32 *ptlen = &((Box *)et->ptShape)->x;
	fp32 ddot, hdot, hlen, hdis;
	Vector3 pos;
	int i, k;
	int inbox = 0;		//在box中：1，不在box中：0

	//pos：射线开始点向量
	xyz_sub(&pos, &ray->start, &et->transform->position);

	for (i = 0; i < 3; i++)
	{
		hdis = xyz_dot16(&pos, &ptRotation[i]);
		hlen = ptlen[i] >> (FP32_FBIT - FP16_FBIT + 1);
		if (hdis < hlen && hdis > -hlen) inbox |= 1 << i;
	}

	for (i = 0; i < 3; i++)
	{
		//不在盒中但现在在两平面内
		if (inbox != 0b111 && (inbox & (1 << i))) continue;
		ptNormal = &ptRotation[i];

		//正法线方向点乘
		ddot = xyz_dot(&ray->direction, ptNormal);
		if (ddot < 1024 && ddot>-1024) continue;

		//在盒外(0)且方向相同(>0)/在盒内(1)且方向相反(<0)时 此面在反向
		if (((inbox & (1 << i)) == 0 && ddot > 0) || ((inbox & (1 << i)) != 0 && ddot < 0))
		{ xyz_neg(&hit->normal, ptNormal); }
		else hit->normal = *ptNormal;

		//pos：面位置
		xyz_add(&pos, &et->transform->position, xyz_scale(&pos, &hit->normal, ptlen[i] >> 1));
		//距离方向点乘
		hdot = xyz_dot(xyz_sub(&hit->tempVector, &pos, &ray->start), ptNormal);
		//ddot、hdot不能异号
		if ((ddot ^ hdot) < 0) continue;

		hit->distance = fp32_div64(hdot, ddot);

		//击中点坐标
		ray_point(&hit->position, ray, hit->distance);
		//pos：平面上击中点向量
		xyz_sub(&pos, &hit->position, &pos);
		k = i == 0 ? 2 : 0;
		hlen = ptlen[k] >> 1;
		hdis = xyz_dot(&pos, &ptRotation[k]);
		if (hdis > hlen || hdis < -hlen) continue;
		hit->u = (FP32_1 + fp32_div32(hdis, hlen)) >> 1;
		k = i == 1 ? 2 : 1;
		hlen = ptlen[k] >> 1;
		hdis = xyz_dot(&pos, &ptRotation[k]);
		if (hdis > hlen || hdis < -hlen) continue;
		hit->v = (FP32_1 + fp32_div32(hdis, hlen)) >> 1;

		hit->et = et;
		//if ((inbox == 0b111 && ddot > 0) || (inbox != 0b111 && ddot < 0)) xyz_neg(&hit->normal, &ptNormal[i]);
		//else hit->normal = ptNormal[i];

		//if (inbox == 0b111) xyz_neg(&hit->normal, &hit->normal);
		return TRUE;	//确认碰撞，返回结果
	}
	return FALSE;
}
#else
int Raycast_Box(Entity *et, Ray *ray, HitPoint *hit)
{
	Vector3 *D, *J, *K;
	Vector3 SP, DK, SJ;
	//Vector3 position;
	fp32 w, h;
	int jd, kd;
	Vector3 *rotVectors = &et->transform.rotation.vectors;

	for (int i = 0; i < 3; i++)
	{
		jd = i == 0 ? 2 : 0;
		kd = i == 1 ? 2 : 1;
		D = &ray->direction;
		J = &rotVectors[jd];
		K = &rotVectors[kd];
		w = *(&((Box *)et->ptShape)->x + jd);
		h = *(&((Box *)et->ptShape)->x + kd);

		xyz_sub(&SP, &ray->start, &et->transform.position);

		xyz_cross(&DK, D, K);
		xyz_cross(&SJ, &SP, J);

		fp32 det = xyz_dot(&DK, J);
		//**det=0?

		hit->u = fp32_div64(xyz_dot(&DK, &SP), det) + (w >> 1);
		if (hit->u < 0 || hit->u > w) continue;
		hit->v = fp32_div64(xyz_dot(&SJ, D), det) + (h >> 1);
		if (hit->v < 0 || hit->v > h) continue;

		hit->distance = fp32_div64(xyz_dot(&SJ, K), det);

		if (hit->quickCheck) return TRUE;

		ray_point(&hit->position, ray, hit->distance);
		hit->normal = rotVectors[i];
		hit->et = et;
		return TRUE;
	}
	return 0;
}
#endif // BOXF


#if 0
int Raycast_Triangle(Entity *et, Ray *ray, HitPoint *hit)
{
	Triangle *tri = (Triangle *)et->ptShape;
	Vector3 E1, E2;
	xyz_sub(&E1, &tri->v1, &tri->v0);
	xyz_sub(&E2, &tri->v2, &tri->v0);
	xyz_normalize(xyz_cross(&hit->normal, &E2, &E1));

	fp32 updot = xyz_dot(&ray->direction, &hit->normal);
	if (updot > -(1 << 10)) return 0;		//不向平面，跳过
	fp32 hdot = xyz_dot(xyz_sub(&hit->tempVector, &tri->v0, &ray->start), &hit->normal);
	if (hdot > 0) return 0;		//在平面下，跳过

	hit->distance = fp32_div64(hdot, updot);
	ray_point(&hit->position, ray, hit->distance);

	Vector3 EP;
	xyz_sub(&EP, &hit->position, &tri->v0);

	fp32 fd = fp32_mul64(E1.x, E2.y) - fp32_mul64(E2.x, E1.y);

	fp32 u = fp32_div32(fp32_mul64(E2.x, EP.y) - fp32_mul64(EP.x, E2.y), -fd);
	//fp32 u = fp32_div64(xyz_dot(&EP, &E1), xyz_len2(&E1));
	if (u < 0 || u > FP32_1) return 0;

	fp32 v = fp32_div32(fp32_mul64(E1.x, EP.y) - fp32_mul64(EP.x, E1.y), fd);
	//fp32 v = fp32_div64(xyz_dot(&EP, &E2), xyz_len2(&E2));
	if (v < 0 || v + u > FP32_1) return 0;
	hit->u = u;
	hit->v = v;
	hit->et = et;
	return 1;
}
#else
int Raycast_Triangle(Entity *et, Ray *ray, HitPoint *hit)
{
	Triangle *tri = (Triangle *)et->ptShape;
	Vector3 E1, E2, DE2, S, SE1;
	fp32 u, v;

	xyz_sub(&E1, &tri->v1, &tri->v0);
	xyz_sub(&E2, &tri->v2, &tri->v0);

	xyz_cross(&DE2, &ray->direction, &E2);
	fp32 det = xyz_dot(&E1, &DE2);
	if (det > 0)
	{
		xyz_sub(&S, &ray->start, &tri->v0);
	}
	else
	{
		xyz_sub(&S, &tri->v0, &ray->start);
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 16) return FALSE;

	// Calculate u and make sure u <= 1
	u = xyz_dot(&S, &DE2);
	if (u < 0 || u > det) return	 FALSE;

	xyz_cross(&SE1, &S, &E1);
	// Calculate v and make sure u + v <= 1
	v = xyz_dot(&ray->direction, &SE1);
	if (v < 0 || u + v > det) return FALSE;

	hit->distance = xyz_dot(&E2, &SE1);
	if (hit->distance < 0) return FALSE;
	//快速检测返回（需要包含距离）
	if (hit->quickCheck) return TRUE;

	fp32 fInvDet = fp32_div64(FP32_1, det);
	hit->distance = fp32_mul64(hit->distance, fInvDet);

	ray_point(&hit->position, ray, hit->distance);
	xyz_normalize(xyz_cross(&hit->normal, &E2, &E1));

	hit->u = fp32_mul64(u, fInvDet);
	hit->v = fp32_mul64(v, fInvDet);
	hit->et = et;
	return TRUE;
}
#endif

#if 0
int Raycast_Mesh(Entity *et, Ray *ray, HitPoint *hit)
{
	Mesh *mesh = (Mesh *)et->ptShape;
	xyz32 E1, E2, EP;
	xyz32 *v0, *v1, *v2;
	int i3;
	for (int i = 0; i < mesh->trianglesCount; i++)
	{
		i3 = i * 3;
		v0 = &mesh->vertices[mesh->triangles[i3]];
		v1 = &mesh->vertices[mesh->triangles[i3 + 1]];
		v2 = &mesh->vertices[mesh->triangles[i3 + 2]];
		//xyz_sub(&E1, v1, v0);
		//xyz_sub(&E2, v2, v0);
		//xyz_normalize(xyz_cross(&hit->normal, &E2, &E1));
		hit->normal = mesh->normals[i];

		fp32 updot = xyz_dot(&ray->direction, &hit->normal);
		if (updot > -(1 << 10)) continue;	//不向平面，跳过
		fp32 hdot = xyz_dot(xyz_sub(&EP, v0, &ray->start), &hit->normal);
		if (hdot > 0) continue;				//在平面下，跳过

		hit->distance = fp32_div64(hdot, updot);
		ray_point(&hit->position, ray, hit->distance);

		xyz_sub(&E1, v1, v0);
		xyz_sub(&E2, v2, v0);
		xyz_sub(&EP, &hit->position, v0);

		fp32 fd = fp32_mul64(E1.x, E2.y) - fp32_mul64(E2.x, E1.y);
		fp32 u = fp32_div32(fp32_mul64(E2.x, EP.y) - fp32_mul64(EP.x, E2.y), -fd);
		if (u < 0 || u > FP32_1) continue;
		fp32 v = fp32_div32(fp32_mul64(E1.x, EP.y) - fp32_mul64(EP.x, E1.y), fd);
		if (v < 0 || v + u > FP32_1) continue;
		hit->u = u;
		hit->v = FP32_1 - v;
		hit->et = et;
		return 1;
	}
	return 0;
}
#else
int Raycast_Mesh(Entity *et, Ray *ray, HitPoint *hit)
{
	Mesh *mesh = (Mesh *)et->ptShape;
	/*
	if (mesh->ptBS != NULL)
	{
		if (Raycast_BSP(mesh->ptBS, ray))
		{
			hit->et = et;
			hit->distance = FP32_1;
			return TRUE;
		}
	}
	*/
	//*
	//包围球快速检测
	if (mesh->ptBS != NULL)
		if (!Raycast_BSP(mesh->ptBS, ray))
			return FALSE;
	//*/
	//int i0, i1, i2;
	Vector3 *v0, *v1, *v2;
	Vector3 E1, E2, DE2, S, SE1;
	fp32 det, fInvDet;
	fp32 distance, u, v;
	hit->distance = FP32_MAX;
	fp32 u_ns, v_ns;
	//击中面的索引
	int i_ns = -1;
	int triCount = mesh->trianglesCount * 3;
	for (int i = 0; i < triCount; i += 3)
	{
		//每隔9个数的前三个为三角面3个顶点索引
		v0 = &mesh->worldVertices[mesh->triangles[i]];
		v1 = &mesh->worldVertices[mesh->triangles[i + 1]];
		v2 = &mesh->worldVertices[mesh->triangles[i + 2]];
		xyz_sub(&E1, v1, v0);
		xyz_sub(&E2, v2, v0);

		xyz_cross(&DE2, &ray->direction, &E2);

		det = xyz_dot(&E1, &DE2);
		if (det > 0)
		{
			//continue;
			xyz_sub(&S, &ray->start, v0);
		}
		else
		{
			xyz_sub(&S, v0, &ray->start);
			det = -det;
		}
		if (det < 16) continue;

		u = xyz_dot(&S, &DE2);
		if (u < 0 || u > det) continue;

		xyz_cross(&SE1, &S, &E1);

		v = xyz_dot(&ray->direction, &SE1);
		if (v < 0 || u + v > det) continue;

		distance = xyz_dot(&E2, &SE1);
		if (distance < 0) continue;

		fInvDet = fp32_div64(FP32_1, det);
		distance = fp32_mul64(distance, fInvDet);

		//是否最近距离的三角面	
		if (distance > hit->distance) continue;
		hit->distance = distance;
		//快速检测返回（需要包含距离）
		if (hit->quickCheck) return TRUE;
		u_ns = fp32_mul64(u, fInvDet);
		v_ns = fp32_mul64(v, fInvDet);
		i_ns = i;
	}

	//没有击中
	if (i_ns == -1) return FALSE;
	//击中，取最近的点
	hit->et = et;
	ray_point(&hit->position, ray, hit->distance);
	//根据3个顶点UV进行线性插值
	Unit2 *uv0 = &mesh->uvs[mesh->triangles[i_ns]];
	Unit2 *uv1 = &mesh->uvs[mesh->triangles[i_ns + 1]];
	Unit2 *uv2 = &mesh->uvs[mesh->triangles[i_ns + 2]];
	//uv = (1-u-v)*uv0 + u*uv1 + v*uv2
	hit->u = uv0->u + fp32_mul64(uv1->u - uv0->u, u_ns) + fp32_mul64(uv2->u - uv0->u, v_ns);
	hit->v = uv0->v + fp32_mul64(uv1->v - uv0->v, u_ns) + fp32_mul64(uv2->v - uv0->v, v_ns);
	//if (mesh->smooth)
	if (TRUE)
	{
		//根据3个顶点法线进行线性插值，结果不是标准向量，需要Normalize  **球面插值应该更好
		v0 = &mesh->worldNormals[mesh->triangles[i_ns]];
		v1 = &mesh->worldNormals[mesh->triangles[i_ns + 1]];
		v2 = &mesh->worldNormals[mesh->triangles[i_ns + 2]];
		//xyz_lerp(&hit->normal, xyz_lerp(&E1, v0, v1, u_ns), v2, v_ns);
		hit->normal.x = v0->x + fp32_mul64(v1->x - v0->x, u_ns) + fp32_mul64(v2->x - v0->x, v_ns);
		hit->normal.y = v0->y + fp32_mul64(v1->y - v0->y, u_ns) + fp32_mul64(v2->y - v0->y, v_ns);
		hit->normal.z = v0->z + fp32_mul64(v1->z - v0->z, u_ns) + fp32_mul64(v2->z - v0->z, v_ns);
	}
	else { hit->normal = mesh->worldNormals[i_ns]; }
	return TRUE;
}
#endif


#if 1
int Raycast_AABB(AABB *aabb, Ray *ray)
{
	Vector3 pos;
	int i, k;
	int inbox = 0;		//在box中：1，不在box中：0

	if (ray->start.x < aabb->max.x && ray->start.x > aabb->min.x) inbox |= 0b001;
	if (ray->start.y < aabb->max.y && ray->start.y > aabb->min.y) inbox |= 0b010;
	if (ray->start.z < aabb->max.z && ray->start.z > aabb->min.z) inbox |= 0b100;
	if (inbox == 0b111) return TRUE;

	fp32 tx0 = fp32_div64(aabb->min.x - ray->start.x, ray->direction.x);
	fp32 tx1 = fp32_div64(aabb->max.x - ray->start.x, ray->direction.x);
	fp32 ty0 = fp32_div64(aabb->min.y - ray->start.y, ray->direction.y);
	fp32 ty1 = fp32_div64(aabb->max.y - ray->start.y, ray->direction.y);
	fp32 tz0 = fp32_div64(aabb->min.z - ray->start.z, ray->direction.z);
	fp32 tz1 = fp32_div64(aabb->max.z - ray->start.z, ray->direction.z);

	fp32 tmax, tmin;
	tmin = tx0 < tx1 ? tx0 : tx1;
	tmax = ty0 > ty1 ? ty0 : ty1;
	if (tmin > tmax) return FALSE;
	tmin = tx0 < tx1 ? tx0 : tx1;
	tmax = tz0 > tz1 ? tz0 : tz1;
	if (tmin > tmax) return FALSE;
	//if ((tx0 < tx1 ? tx0 : tx1) > (tz0 > tz1 ? tz0 : tz1)) return FALSE;
	return TRUE;

	for (i = 0; i < 3; i++)
	{
		//不在盒中但现在在两平面内
		if (inbox & (1 << i)) continue;

		if (ray->direction.x < 32 && ray->direction.x > -32 && inbox == 0b110) return TRUE;

		fp32 t1 = fp32_div64(aabb->min.x - ray->start.x, ray->direction.x);
		fp32 t2 = fp32_div64(aabb->max.x - ray->start.x, ray->direction.x);


		return TRUE;	//确认碰撞，返回结果
	}
	return FALSE;
}
#else

#endif

int Raycast_BSP(BSP *bs, Ray *ray)
{
	//球半径r
	fp32 r2 = fp32_pow2(bs->radius);

	//出射点S 到球心P
	Vector3 SP;
	//入射点到球心距离平方 sp2
	fp32 sp2 = xyz_len2(xyz_sub(&SP, &bs->position, &ray->start));

	//在球内，必相交
	if (sp2 < r2) return TRUE;

	//出射点S 到 射线上 距球心最近点D 的长度
	fp32 sd = xyz_dot(&SP, &ray->direction);

	//（出射点在球外）交点不在射线方向，返回
	if (sd < 0) return FALSE;

	//球心P 到最近点D 的长度平方 (pd2 = sp2 - sd2)
	fp32 pd2 = sp2 - fp32_pow2(sd);

	//射线与圆心距离大于半径，返回
	if (pd2 > r2) return FALSE;

	return TRUE;
}