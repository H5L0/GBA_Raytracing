#pragma once
#include "RT_base.h"

typedef struct Animator Animator;
typedef struct AnimationInterface AnimationInterface;
typedef struct Animation Animation;
typedef struct Clip Clip;


//动画控制器
struct Animator
{
	int animationCount;
	Animation *animations[0];
};

//动画控制的属性
enum AnimationProperty
{
	AP_None,
	AP_Position,
	AP_Rotation,
	AP_Scale,
	AP_
};

/*----------------------------动画定义------------------------------*/

//动画信息 基类
struct Animation
{
	AnimationInterface *fc;
	//...
};

/**
//动画信息 方法
MethodDefine(void, ResetAnimation, Animation *animation, Scene *scene)
MethodDefine(void, PlayAnimation, Animation *animation, Scene *scene, int frame)
MethodDefine(void, StepAnimation, Animation *animation, Scene *scene)

//动画信息 方法接口
struct AnimationInterface
{
	ResetAnimationFC Reset;		//重置场景里所有物体动画到0帧
	PlayAnimationFC Play;		//播放动画到f帧
	StepAnimationFC Step;			//播放动画下一帧
};

//自动创建动画信息的方法接口
#define AnimationInterfaceCode(Name)								\
void ResetAnimation_##Name(Animation *animation,Scene *scene);	\
void PlayAnimation_##Name(Animation *animation,Scene *scene, int frame); \
void StepAnimation_##Name(Animation *animation,Scene *scene);	\
static AnimationInterface AnimationInterface_##Name =			\
{																		\
	ResetAnimation_##Name,											\
	PlayAnimation_##Name,											\
	StepAnimation_##Name,											\
};
//\**static?
*/

#define ARGS_ResetAnimation (Animation *animation, Scene *scene)
#define RETURN_ResetAnimation void
#define ARGS_PlayAnimation (Animation *animation, Scene *scene,int frame)
#define RETURN_PlayAnimation void
#define ARGS_StepAnimation (Animation *animation, Scene *scene)
#define RETURN_StepAnimation void

struct AnimationInterface
{
	FC_PTR(ResetAnimation, Reset);
	FC_PTR(PlayAnimation, Play);
	FC_PTR(StepAnimation, Step);
};

//自动创建动画信息的方法接口
#define AnimationInterfaceCode(Name)	\
FC_DEF(ResetAnimation,Name);	\
FC_DEF(PlayAnimation,Name);		\
FC_DEF(StepAnimation,Name);		\
static AnimationInterface AnimationInterface_##Name =	\
{																\
	ResetAnimation_##Name,									\
	PlayAnimation_##Name,									\
	StepAnimation_##Name,									\
};

//创建动画机
Animator *CreateAnimator(int animationCount);
//创建动画机，可变参数
Animator *CreateAnimatorVA(int animationCount, ...);

//播放下一帧动画
void Animator_Play(Animator *animator, Scene *scene, int frame);
void Animator_Step(Animator *animator, Scene *scene);
void Animator_Reset(Animator *animator, Scene *scene);


/*------------------------------表达式动画-------------------------------*/

//表达式动画需要实现的表达式方法
//MethodDefine(void, ExpressionAnimation, Scene *scene, int frame)
typedef void (*ExpressionAnimationFC) (Scene *scene, int frame);

//通过代码控制动画的动画信息 继承自Animation
typedef struct ExpressionAnimation
{
	Animation base;
	int expressionCount;
	ExpressionAnimationFC expressions[0];
}ExpressionAnimation;

AnimationInterfaceCode(Expression)	//定义表达式动画的接口，实现Animation的方法


//创建表达式动画
ExpressionAnimation *CreateExpressionAnimation(int expressionCount);
//创建表达式动画，可变参数
ExpressionAnimation *CreateExpressionAnimationVA(int count, ...);



/*------------------------------曲线动画-------------------------------*/

//通过曲线控制动画的接口实现
typedef struct
{
	Animation base;

	//动画对象
	int entityID;
	enum AnimationProperty property;

	//动画段
	Clip *clips;
}CurveAnimation;



typedef struct KEYPOINT_1D
{
	int time;
	fp32 value;

	fp16 inTargent;
	fp16 inTensor;
	fp16 outTargent;
	fp16 outTensor;
}KeyPoint, KeyPoint1D;


typedef struct KEYPOINT_2D
{
	int time;
	fp32 value[2];
	fp16 inTargent[2];
	fp16 inTensor;
	fp16 outTargent[2];
	fp16 outTensor;
}KeyPoint2D;


typedef struct KEYPOINT_3D
{
	int time;
	fp32 value[3];
	fp16 inTargent[3];
	fp16 inTensor;
	fp16 outTargent[3];
	fp16 outTensor;
}KeyPoint3D;

typedef struct
{
	int keyCount;
	KeyPoint keys[0];
}Curve;

typedef struct
{
	int keyCount;
	KeyPoint3D keys[0];
}Curve3D;

struct Clip
{
	int time;
	int length;

	Curve curve;

};

