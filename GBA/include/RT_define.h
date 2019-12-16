#pragma once
#include "RT_base.h"

//漫反射
Color Trace(Scene *sc, Ray *ray);
Color GetLightIrradiant(Scene *sc, HitPoint *hit);
fp16 GetAmbientIllumination(Scene *sc, HitPoint *hit);
Vector3 *GetSkyLights();

//光线追踪主程序
void RayTracing(Scene *sc, int targetSize);
void RayTracing_Adv(Scene *sc, int targetsize);
void RayTracing_Ani(Scene *sc, int targetSize);
void RayTracing_Pattern(Scene *sc, int targetSize);
void RayTracing_Fly(Scene *sc, int targetSize);
void RayTracing_16(Scene *sc, int line_start, int line_end);

//定义在Scene.c
Scene *GetTestScene();
Camera *GetTestCamera();	//获取测试摄像机
int GetTestSceneObject(Scene *sc);		//获取测试场景物体
int GetTestSceneLight(Scene *sc);		//获取测试场景灯光
void RotateCameraAround(Camera *cam, int distance, int frame);
Scene *GetEarthScene();
Scene *GetSaturnScene();
Color GetSunSkyColor(Vector3 *dir);
Color GetGalaxyColor(Vector3 *dir);
Color GetBlackBackground(Vector3 *dir);
Color GetSpaceColor(Vector3 *dir);

//移动控制
int NeedUpdateController();
void InitFPSController(Scene *sc);
void FPSControllerUpdate(Scene *sc);
void InitTargetController(Scene *sc);
void TargetControllerUpdate(Scene *sc);

//场景表达式动画
void RotatingAnimation_Camera(Scene *scene, int frame);
void RotatingAnimation_Transforms(Scene *scene, int frame);


