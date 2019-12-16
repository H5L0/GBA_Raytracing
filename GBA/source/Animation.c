#include "RT_base.h"
#include "Transform.h"
#include "Animation.h"

Animator *CreateAnimator(int animationCount)
{
	Animator *animator = mem_alloc_i(sizeof(Animator) + sizeof(Animation *));
	animator->animationCount = animationCount;
	return animator;
}

Animator *CreateAnimatorVA(int animationCount, ...)
{
	Animator *animator = CreateAnimator(animationCount);
	for (int i = 0; i < animationCount; i++)
	{
		animator->animations[i] = *(Animation **)(&animationCount + i + 1);
	}
	return animator;
}


void Animator_Play(Animator *animator, Scene *scene, int frame)
{
	Animation *animation;
	//_animator_frame = frame;
	for (int i = 0; i < animator->animationCount; i++)
	{
		animation = animator->animations[i];
		animation->fc->Play(animation, scene, frame);
	}
}

//static int _animator_frame = 0;
void Animator_Step(Animator *animator, Scene *scene)
{
	Animation *animation;
	for (int i = 0; i < animator->animationCount; i++)
	{
		animation = animator->animations[i];
		animation->fc->Step(animation, scene);
	}
}

void Animator_Reset(Animator *animator, Scene *scene)
{
	Animation *animation;
	for (int i = 0; i < animator->animationCount; i++)
	{
		animation = animator->animations[i];
		animation->fc->Reset(animation, scene);
	}
}



//创建表达式动画
ExpressionAnimation *CreateExpressionAnimation(int expressionCount)
{
	ExpressionAnimation *exprAni = mem_alloc_i(sizeof(ExpressionAnimation) + sizeof(ExpressionAnimationFC) * expressionCount);
	exprAni->base.fc = &AnimationInterface_Expression;
	exprAni->expressionCount = expressionCount;
	return exprAni;
}

ExpressionAnimation *CreateExpressionAnimationVA(int count, ...)
{
	ExpressionAnimation *exprAni = CreateExpressionAnimation(count);
	for (int i = 0; i < count; i++)
	{
		exprAni->expressions[i] = *(ExpressionAnimationFC *)(&count + i + 1);
	}
	return exprAni;
}


FC_DEF(ResetAnimation, Expression)
{
	PlayAnimation_Expression(animation, scene, 0);
}

static int _expressionFrame = 0;
FC_DEF(PlayAnimation, Expression)
{
	_expressionFrame = frame;
	ExpressionAnimation *expr = (ExpressionAnimation *)animation;
	for (int i = 0; i < expr->expressionCount; i++)
	{
		expr->expressions[i](scene, frame);
	}
}

FC_DEF(StepAnimation, Expression)
{
	PlayAnimation_Expression(animation, scene, _expressionFrame);
	_expressionFrame++;
}



/*
typedef struct CLIP
{
	int time;
	int length;

	int posStart;
	int rotStart;

	//0/1表示有无数据
	u32 *posSteps;
	u32 *rotSteps;
}Clip;


typedef struct ANIMATION
{
	//动画对象
	int targetID;
	Transform target;

	int nowClip;
	int nowStep;

	int nowPos;
	int nowRot;

	//动画段
	Clip *clips;

	//动画数据
	Vector3 *positions;
	//4个向量一个旋转基坐标系，（3？）
	s16 *rotations;
}Animation;


typedef struct SCC
{
	int amount;
	Animation animations[8];
}SceneAnimation;


int nowTime;
SceneAnimation animations;


void InitAnimation()
{
	Animation *ani;

	for (int i = 0; i < animations.amount; i++)
	{
		ani = &animations.animations[i];
		ani->nowClip = 0;
		ani->nowStep = -1;
		ani->nowPos = -1;
		ani->nowRot = -1;
	}
}


void PlayNextFrame()
{
	nowTime++;
	Animation *ani;
	Clip *clip;
	Rotation rot;

	for (int i = 0; i < animations.amount; i++)
	{
		ani = &animations.animations[i];
		clip = &ani->clips[ani->nowClip];

		//已播放完此动画段
		if (ani->nowStep == clip->length - 1)
		{
			ani->nowClip++;
			ani->nowStep = -1;
			clip++;
		}

		//现在时间大于此片段开始时间
		if (nowTime > clip->time) continue;

		ani->nowStep++;

		int u32Index = ani->nowStep >> 5;
		int bitMask = 1 << (ani->nowStep & 0b11111);
		if (clip->posSteps[u32Index] & bitMask) ani->nowPos++;
		if (clip->rotSteps[u32Index] & bitMask) ani->nowRot++;

		ani->target.position = ani->positions[ani->nowPos];
		//getRot;
		//ani->target.rotation = rot;

	}

}
*/