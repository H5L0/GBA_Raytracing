#pragma once
#include <hl_math.h>

typedef struct TRANSFORM Transform;
typedef struct CHILDREN Children;
typedef struct ENTITY Entity;

//1+1+3+9+3+3+9+3 = 32
typedef struct TRANSFORM
{
	Transform *parent;
	Children *children;
	Entity *entity;
	Vector3 position;
	Rotation rotation;
	Vector3 scale;
	Vector3 localPosition;
	Rotation localRotation;
	Vector3 localScale;
}Transform;

typedef struct CHILDREN
{
	int count;
	Transform *list[0];
}Children;


Transform *InitTransform(Transform *tf);
Transform *SetTransform(Transform *transform, Vector3 *pos, Rotation *rot, Transform *parent);

void _SetPosition(Transform *transform, Vector3 *pos);
void _SetLocalPosition(Transform *transform, Vector3 *pos);
void _UpdateChildrenPosition(Transform *parent);

void _SetRotation(Transform *transform, Rotation *rot);
void _SetLocalRotation(Transform *transform, Rotation *rot);
void _UpdateChildrenRotation(Transform *parent);

void _SetChild(Transform *parent, int index, Transform *child);
void _AddChild(Transform *parent, Transform *child);

Transform *RotateTransform(Transform *tf, fp32 x, fp32 y, fp32 z);

static inline void UpdatePosition(Transform *transform) { _SetPosition(transform, NULL); }
static inline void UpdateRotation(Transform *transform) { _SetRotation(transform, NULL); }
static inline void UpdateTransform(Transform *transform) { _SetRotation(transform, NULL); }