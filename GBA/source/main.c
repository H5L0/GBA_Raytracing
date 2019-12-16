#include <hl_system.h>
#include <hl_gba.h>
#include <hl_input.h>
#include <hl_timers.h>
#include <hl_color.h>
#include <hl_graphics.h>
#include <hl_math.h>
#include <hl_memory.h>
#include <hl_debug.h>
#include "RT_base.h"
#include "RT_define.h"
#include "RT_data.h"
#include "Animation.h"
#include "Shape.h"

#include "scene_Voyoger.h"
//#include "scene_TestFile.h"

int main()
{
	mem_heap_init();
	SetMode(MODE_3 | BG2_ON);
	//PrintTextInMode3("Ray Tracing Test 010", 32, 64, RGB5_WHITE);

	//TestAABB();
	//while (1);

	//Scene *scene = GetTestScene();
	Scene *scene = GetEarthScene();
	//Scene *scene = GetSaturnScene();
	//Scene *scene = LoadScene(scene_Voyoger);
	//Scene *scene = LoadScene(scene_TestFile);
	//GetSkyLights();

	//创建测试动画组件
	ExpressionAnimation *expr = CreateExpressionAnimationVA(2, RotatingAnimation_Camera, RotatingAnimation_Transforms);
	//创建动画机
	scene->animator = CreateAnimatorVA(1, expr);

	//while (1);

	int frameCount = 220;
	while (1)
	{
		//PrintIntInMode3(frameCount, 4, 148);		//绘制帧计数
		Animator_Play(scene->animator, scene, frameCount);

		//RayTracing(scene, 1);
		//RayTracing_Adv(scene, 1);	
		//RayTracing_Ani(scene, 16);
		//RayTracing_Pattern(scene,1);	
		RayTracing_Fly(scene, 1);

		frameCount++;
	}
	return 0;
}


