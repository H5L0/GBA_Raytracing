#include <hl_gba.h>
#include "RT_base.h"


Entity *SetEntity(Entity *et, Transform *transform, BaseShape *ptShape, Material *mt)
{
	et->property = EP_Default;
	/**Transform指针之后设定需要分配内存**/
	//**设定坐标
	//et->transform->parent = NULL;
	//et->transform->children = NULL;
	//_SetPosition(et->transform, pos);
	//_SetRotation(et->transform, rot);
	transform->entity = et;
	et->transform = transform;
	et->ptShape = ptShape;
	et->ptMaterial = mt;
	return et;
}


Camera *CreateCamera(Vector3 *pos, Vector3 *target, fp32 size)
{
	Camera *cam = mem_alloc_i(sizeof(Camera) + sizeof(Transform));
	cam->property = 0;
	cam->transform = (Transform *)(cam + 1);
	InitTransform(cam->transform);
	//设定位置
	_SetPosition(cam->transform, pos);
	//在此设定rotation的值
	SetCameraTarget(cam, target);
	cam->size_x = size;
	cam->size_y = SCREEN_Y * size / SCREEN_X;
	return cam;
}


//初始化摄像机
//size (0~16)：水平视野（摄像机视角 = 2 * atan(size / 2) (1->53.12deg, 2->90deg)
void InitCamera(Camera *cam, Vector3 *pos, Rotation *rot, fp32 size)
{
	_SetPosition(cam->transform, pos);
	_SetRotation(cam->transform, rot);
	cam->size_x = size;
	cam->size_y = SCREEN_Y * size / SCREEN_X;
}

//从屏幕像素位置产生一条射线
Ray *ScreenToRay(Camera *cam, Ray *ray, int x, int y)
{
	ray->start = cam->transform->position;
	Vector3 dir;
	//cam->size变小后，精度变低，不能低于2^13
	dir.x = ((x - (SCREEN_X / 2)) * cam->size_x) / SCREEN_X;
	dir.y = (((SCREEN_Y / 2) - y) * cam->size_y) / SCREEN_Y;
	dir.z = fp32_int(1);
	TransformToWorld(&cam->transform->rotation, &dir, &ray->direction);
	//最后标准化才能保证精度
	xyz_normalize(&ray->direction);
	ray->depth = 0;
	ray->weigth = FP32_1;
	return ray;
}

//根据目标设定摄像机方向 不会改变target
void SetCameraTarget(Camera *cam, Vector3 *target)
{
	Vector3 direction;
	xyz_normalize(xyz_sub(&direction, target, &cam->transform->position));
	SetRotationForward(&cam->transform->rotation, &direction);
	/*
	fp32 xxzz = fp32_sqrt64(fp32_pow2(rot->forward.x) + fp32_pow2(rot->forward.z));
	rot->up.x = fp32_mul64(-rot->forward.y, fp32_div64(rot->forward.x, xxzz));
	rot->up.y = xxzz;
	rot->up.z = fp32_mul64(-rot->forward.y, fp32_div64(rot->forward.z, xxzz));
	xyz_cross(&rot->right, &rot->up, &rot->forward);*/
}