#pragma once
#include "RT_base.h"

typedef struct Animator Animator;
typedef struct AnimationInterface AnimationInterface;
typedef struct Animation Animation;
typedef struct Clip Clip;


//����������
struct Animator
{
	int animationCount;
	Animation *animations[0];
};

//�������Ƶ�����
enum AnimationProperty
{
	AP_None,
	AP_Position,
	AP_Rotation,
	AP_Scale,
	AP_
};

/*----------------------------��������------------------------------*/

//������Ϣ ����
struct Animation
{
	AnimationInterface *fc;
	//...
};

/**
//������Ϣ ����
MethodDefine(void, ResetAnimation, Animation *animation, Scene *scene)
MethodDefine(void, PlayAnimation, Animation *animation, Scene *scene, int frame)
MethodDefine(void, StepAnimation, Animation *animation, Scene *scene)

//������Ϣ �����ӿ�
struct AnimationInterface
{
	ResetAnimationFC Reset;		//���ó������������嶯����0֡
	PlayAnimationFC Play;		//���Ŷ�����f֡
	StepAnimationFC Step;			//���Ŷ�����һ֡
};

//�Զ�����������Ϣ�ķ����ӿ�
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

//�Զ�����������Ϣ�ķ����ӿ�
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

//����������
Animator *CreateAnimator(int animationCount);
//�������������ɱ����
Animator *CreateAnimatorVA(int animationCount, ...);

//������һ֡����
void Animator_Play(Animator *animator, Scene *scene, int frame);
void Animator_Step(Animator *animator, Scene *scene);
void Animator_Reset(Animator *animator, Scene *scene);


/*------------------------------���ʽ����-------------------------------*/

//���ʽ������Ҫʵ�ֵı��ʽ����
//MethodDefine(void, ExpressionAnimation, Scene *scene, int frame)
typedef void (*ExpressionAnimationFC) (Scene *scene, int frame);

//ͨ��������ƶ����Ķ�����Ϣ �̳���Animation
typedef struct ExpressionAnimation
{
	Animation base;
	int expressionCount;
	ExpressionAnimationFC expressions[0];
}ExpressionAnimation;

AnimationInterfaceCode(Expression)	//������ʽ�����Ľӿڣ�ʵ��Animation�ķ���


//�������ʽ����
ExpressionAnimation *CreateExpressionAnimation(int expressionCount);
//�������ʽ�������ɱ����
ExpressionAnimation *CreateExpressionAnimationVA(int count, ...);



/*------------------------------���߶���-------------------------------*/

//ͨ�����߿��ƶ����Ľӿ�ʵ��
typedef struct
{
	Animation base;

	//��������
	int entityID;
	enum AnimationProperty property;

	//������
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

