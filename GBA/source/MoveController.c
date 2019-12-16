#include <hl_input.h>
#include <hl_math.h>
#include "RT_base.h"
#include "Material.h"


Vector3 camTarget = { 0, 0, 0 };
Vector3 movement = { 0, 0, 0 };
Vector3 movement_world = { 0, 0, 0 };

void InitFPSController(Scene *sc)
{
	Transform *tf = sc->camera->transform;
	tf->position.x = 0;
	tf->position.y = fp32_int(1);
	tf->position.z = fp32_int(-4);
	camTarget = tf->position;
	camTarget.z += FP32_1;
}

//检查控制器是否有输入
int NeedUpdateController()
{
	return GetNewInput();
}


void FPSControllerUpdate(Scene *sc)
{
	Transform *tf = sc->camera->transform;
	movement.x = GetAxis_Horizontal() << (FP32_FBIT - 3);
	movement.y = GetAxis_Vertical() << (FP32_FBIT - 3);
	movement.z = 0;

	TransformToWorld(&tf->rotation, &movement, &movement_world);
	xyz_add(&camTarget, &camTarget, &movement_world);

	if (GetAxis_AB())
	{
		xyz_scale(&movement_world, &tf->rotation.forward, GetAxis_AB() << (FP32_FBIT - 3));
		xyz_add(&tf->position, &tf->position, &movement_world);
		xyz_add(&camTarget, &camTarget, &movement_world);
	}

	SetCameraTarget(sc->camera, &camTarget);
}


void InitTargetController(Scene *sc)
{
	camTarget.y = FP32_H5;
}

void TargetControllerUpdate(Scene *sc)
{
	Transform *camtf = sc->camera->transform;
	fp32 dis = xyz_len(xyz_sub(&movement, &camTarget, &camtf->position));
	movement.x = GetAxis_Horizontal() * (dis >> 6);
	movement.y = GetAxis_Vertical() * (dis >> 6);
	movement.z = GetAxis_LR() * (dis >> 6);
	if (GetKeyDown(KEY_A))
	{
		//移动镜头目标
		TransformToWorld(&camtf->rotation, &movement, &movement_world);
		xyz_add(&camTarget, &camTarget, &movement_world);
		SetCameraTarget(sc->camera, &camTarget);
	}
	else if (GetKeyDown(KEY_B))
	{
		Transform *tf = sc->entities[2].transform;
		xyz_add(&tf->position, &tf->position, &movement);

		_SetPosition(tf, &tf->position);
	}
	else
	{
		//移动镜头
		TransformToWorld(&camtf->rotation, &movement, &movement_world);
		xyz_add(&camtf->position, &camtf->position, &movement_world);
		SetCameraTarget(sc->camera, &camTarget);
	}

	if (GetKeyDown(KEY_SELECT))
	{
		for (int i = 0; i < sc->objectCount; i++)
		{
			((MT_Reflect *)sc->entities[i].ptMaterial)->reflectance -= fp16_100f(5);
			if (((MT_Reflect *)sc->entities[i].ptMaterial)->reflectance < 0)
				((MT_Reflect *)sc->entities[i].ptMaterial)->reflectance = 0;
		}

	}
	else if (GetKeyDown(KEY_START))
	{
		for (int i = 0; i < sc->objectCount; i++)
		{
			((MT_Reflect *)sc->entities[i].ptMaterial)->reflectance += fp16_100f(5);
			if (((MT_Reflect *)sc->entities[i].ptMaterial)->reflectance > FP16_1)
				((MT_Reflect *)sc->entities[i].ptMaterial)->reflectance = FP16_1;
		}
	}
}