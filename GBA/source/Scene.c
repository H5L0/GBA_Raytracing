#include <hl_memory.h>
#include <hl_color.h>
#include "RT_base.h"
#include "RT_define.h"


Color GetBlackBackground(Vector3 *dir)
{
	Color c = { 0, 0, 0 };
	return c;
}

Scene *CreateScene(int entityCapacity)
{
	Scene *sc = mem_alloc_i(sizeof(Scene) + sizeof(Entity) * entityCapacity);
	Transform *tfs = mem_alloc_i(sizeof(Transform) * entityCapacity);
	sc->transforms = tfs;
	for (int i = 0; i < entityCapacity; i++)
	{
		//sc->entities[i].transform = &tfs[i];
		InitTransform(&tfs[i]);
	}
	return sc;
}


void RotateCameraAround(Camera *cam, int distance, int frame)
{
	frame = frame << (FP32_FBIT - 6);
	cam->transform->position.x = sin_ht(frame) * distance * 2;
	cam->transform->position.z = -cos_ht(frame) * distance;
	cam->transform->position.y = fp32_100f(150);
	fp32 h = sin_ht(frame << 1) >> 1;
	Vector3 direction = { 0, h, h * 2 };
	SetCameraTarget(cam, &direction);
}


void RotatingAnimation_Scene(Scene *scene, int frame)
{
	//
}

void RotatingAnimation_Camera(Scene *scene, int frame)
{
	RotateCameraAround(scene->camera, 8, frame);
}


void RotatingAnimation_Transforms(Scene *scene, int frame)
{
	Transform *tf = &scene->transforms[0];
	int cycle = 120;
	fp32 time = (frame % cycle) * FP32_1 / cycle;
	fp32 value = sin_ht(time * 4) / cycle;
	tf->position.y += value << 1;

	//Rotate(&tf->rotation, 0, fp32_100f(2), 0);
	Rotate(&tf->rotation, fp32_100f(2), fp32_100f(2), 0);

	//**Ö´ÐÐË³Ðò£¿£¿
	//_SetPosition(tf, NULL);
	UpdateTransform(tf);
}






void _UpdateSceneObject(Scene *ptScene, int frame)
{
	Transform *tf = ptScene->entities[2].transform;
	//Vector3 icr = { 0, frame * FP32_1 / 20 + FP32_H5, 0 };
	Vector3 icr = { 0, FP32_1, 0 };
	_SetPosition(tf, &icr);

	Vector3 objTarget = { -frame << (FP32_FBIT - 2), 0, FP32_1 * 5 };
	//Vector3 objTarget = { 0, FP32_1 * 8 , FP32_1 * 5 };
	Vector3 direction;
	xyz_normalize(xyz_sub(&direction, &objTarget, &tf->position));
	SetRotationForward(&tf->rotation, &direction);

	_SetRotation(tf, NULL);
}

void _UpdateCameraTest(Scene *ptScene, int frame)
{
	Camera *cam = ptScene->camera;
	int distance = 4;
	//frame = frame << (FP32_FBIT - 8);
	frame = frame << (FP32_FBIT - 6);
	//cam->position->x = sin_ht(frame) * distance;
	//cam->position->z = -cos_ht(frame) * distance;
	//cam->position->y = fp32_100f(150);
	cam->transform->position.y = frame * distance + FP32_H5;

	Vector3 direction = { 0, FP32_1 + frame * distance / 2, 0 };
	SetCameraTarget(cam, &direction);
}