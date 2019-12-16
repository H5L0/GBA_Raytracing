#include <hl_gba.h>
#include "RT_base.h"


Entity *SetEntity(Entity *et, Transform *transform, BaseShape *ptShape, Material *mt)
{
	et->property = EP_Default;
	/**Transformָ��֮���趨��Ҫ�����ڴ�**/
	//**�趨����
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
	//�趨λ��
	_SetPosition(cam->transform, pos);
	//�ڴ��趨rotation��ֵ
	SetCameraTarget(cam, target);
	cam->size_x = size;
	cam->size_y = SCREEN_Y * size / SCREEN_X;
	return cam;
}


//��ʼ�������
//size (0~16)��ˮƽ��Ұ��������ӽ� = 2 * atan(size / 2) (1->53.12deg, 2->90deg)
void InitCamera(Camera *cam, Vector3 *pos, Rotation *rot, fp32 size)
{
	_SetPosition(cam->transform, pos);
	_SetRotation(cam->transform, rot);
	cam->size_x = size;
	cam->size_y = SCREEN_Y * size / SCREEN_X;
}

//����Ļ����λ�ò���һ������
Ray *ScreenToRay(Camera *cam, Ray *ray, int x, int y)
{
	ray->start = cam->transform->position;
	Vector3 dir;
	//cam->size��С�󣬾��ȱ�ͣ����ܵ���2^13
	dir.x = ((x - (SCREEN_X / 2)) * cam->size_x) / SCREEN_X;
	dir.y = (((SCREEN_Y / 2) - y) * cam->size_y) / SCREEN_Y;
	dir.z = fp32_int(1);
	TransformToWorld(&cam->transform->rotation, &dir, &ray->direction);
	//����׼�����ܱ�֤����
	xyz_normalize(&ray->direction);
	ray->depth = 0;
	ray->weigth = FP32_1;
	return ray;
}

//����Ŀ���趨��������� ����ı�target
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