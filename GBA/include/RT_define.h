#pragma once
#include "RT_base.h"

//������
Color Trace(Scene *sc, Ray *ray);
Color GetLightIrradiant(Scene *sc, HitPoint *hit);
fp16 GetAmbientIllumination(Scene *sc, HitPoint *hit);
Vector3 *GetSkyLights();

//����׷��������
void RayTracing(Scene *sc, int targetSize);
void RayTracing_Adv(Scene *sc, int targetsize);
void RayTracing_Ani(Scene *sc, int targetSize);
void RayTracing_Pattern(Scene *sc, int targetSize);
void RayTracing_Fly(Scene *sc, int targetSize);
void RayTracing_16(Scene *sc, int line_start, int line_end);

//������Scene.c
Scene *GetTestScene();
Camera *GetTestCamera();	//��ȡ���������
int GetTestSceneObject(Scene *sc);		//��ȡ���Գ�������
int GetTestSceneLight(Scene *sc);		//��ȡ���Գ����ƹ�
void RotateCameraAround(Camera *cam, int distance, int frame);
Scene *GetEarthScene();
Scene *GetSaturnScene();
Color GetSunSkyColor(Vector3 *dir);
Color GetGalaxyColor(Vector3 *dir);
Color GetBlackBackground(Vector3 *dir);
Color GetSpaceColor(Vector3 *dir);

//�ƶ�����
int NeedUpdateController();
void InitFPSController(Scene *sc);
void FPSControllerUpdate(Scene *sc);
void InitTargetController(Scene *sc);
void TargetControllerUpdate(Scene *sc);

//�������ʽ����
void RotatingAnimation_Camera(Scene *scene, int frame);
void RotatingAnimation_Transforms(Scene *scene, int frame);


