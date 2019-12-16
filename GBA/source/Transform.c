#include "RT_base.h"


Transform *InitTransform(Transform *tf)
{
	tf->entity = NULL;
	tf->parent = NULL;
	tf->children = NULL;
	return tf;
}


Transform *SetTransform(Transform *tf, Vector3 *pos, Rotation *rot, Transform *parent)
{
	tf->parent = parent;

	_SetPosition(tf, pos);

	if (rot == NULL) InitRotation(&tf->rotation);
	else _SetRotation(tf, rot);

	return tf;
}


//设定世界坐标Position
//如果pos==NULL,则只更新局部坐标LocalPosition
void _SetPosition(Transform *transform, Vector3 *pos)
{
	//设定世界坐标
	if (pos == NULL) pos = &transform->position;
	else transform->position = *pos;

	//更新本地坐标
	if (transform->parent == NULL) transform->localPosition = *pos;
	else
	{
		TransformPointToLocal(&transform->parent->rotation, &transform->parent->position, pos, &transform->localPosition);
	}

	if (transform->entity != NULL)
		transform->entity->ptShape->fc->OnTransformUpdate(transform->entity);

	//更新子对象世界坐标
	if (transform->children != NULL)
		_UpdateChildrenPosition(transform);
}

//设定局部坐标LocalPosition
//如果pos==NULL,则只更新世界坐标Position
void _SetLocalPosition(Transform *transform, Vector3 *pos)
{
	//设定局部坐标
	if (pos == NULL) pos = &transform->localPosition;
	else transform->localPosition = *pos;

	//更新世界坐标
	if (transform->parent == NULL) transform->position = *pos;
	else
	{
		//从局部坐标转换到世界坐标,局部坐标以此物体的世界旋转为基坐标(Rotation)
		TransformPointToWorld(&transform->parent->rotation, &transform->parent->position, pos, &transform->position);
	}

	if (transform->entity != NULL)
		transform->entity->ptShape->fc->OnTransformUpdate(transform->entity);

	//更新子对象世界坐标
	if (transform->children != NULL)
		_UpdateChildrenPosition(transform);
}

//更新子对象的世界坐标
void _UpdateChildrenPosition(Transform *parent)
{
	/*
	//更新子对象世界坐标
	int count = parent->children->count;
	Transform *child;
	Vector3 temp;
	for (int i = 0; i < count; i++)
	{
		child = parent->children->list[i];

		//设定世界坐标
		//TransformToWorld(&parent->rotation, &child->localPosition, &temp);
		//xyz_add(&child->position, &parent->position, &temp);

		//if (child->entity != NULL)
		//	child->entity->ptShape->fc->OnTransformUpdate(child->entity);

		//if (child->children != NULL) _UpdateChildrenPosition(child);
	}*/

	int count = parent->children->count;
	for (int i = 0; i < count; i++)
	{
		//设定子对象坐标（递归）不需要更新local
		_SetLocalPosition(parent->children->list[i], NULL);
	}
}


//设定世界旋转基坐标系
void _SetRotation(Transform *transform, Rotation *rot)
{
	if (rot == NULL) rot = &transform->rotation;
	else transform->rotation = *rot;

	//更新本地基坐标系
	if (transform->parent == NULL) transform->localRotation = *rot;
	else
	{
		TransformMatrixToLocal(&transform->parent->rotation, rot, &transform->localRotation);
		//cross
	}

	if (transform->children != NULL)
	{
		_UpdateChildrenRotation(transform);
		_UpdateChildrenPosition(transform);
		//只要世界旋转发生变化，就要更新子对象的Position
	}
}

//设定本地旋转基坐标系
//本地旋转坐标系储存以 父对象世界旋转矩阵 为基的坐标系
void _SetLocalRotation(Transform *transform, Rotation *rot)
{
	if (rot == NULL) rot = &transform->localRotation;
	else transform->localRotation = *rot;

	//更新世界基坐标系
	if (transform->parent == NULL) transform->rotation = *rot;
	else
	{
		TransformMatrixToWorld(&transform->parent->rotation, rot, &transform->rotation);
	}

	if (transform->children != NULL)
	{
		_UpdateChildrenRotation(transform);
		_UpdateChildrenPosition(transform);
		//只要世界旋转发生变化，就要更新子对象的Position
	}
}


void _UpdateChildrenRotation(Transform *parent)
{
	int count = parent->children->count;
	for (int i = 0; i < count; i++)
	{
		//只需要更新子对象的世界基坐标系
		_SetLocalRotation(parent->children->list[i], NULL);
		//_SetRotation(parent->children->list[i], NULL);
	}
}




void _SetChild(Transform *parent, int index, Transform *child)
{
	parent->children->list[index] = child;
	child->parent = parent;
	_SetPosition(child, &child->position);
	_SetRotation(child, NULL);
}

void _AddChild(Transform *parent, Transform *child)
{
	//已有children的数量
	int count = 0;
	if (parent->children != NULL) count = parent->children->count;

	//void *list = mem_alloc_i(sizeof(int) + count * sizeof(Transform *));
	//*(int *)list = count;
	//Transform **children = (Transform **)((char *)list + sizeof(int));

	Children *children = mem_alloc_i(sizeof(Children) + (count + 1) * sizeof(Transform *));
	children->count = count + 1;

	for (int i = 0; i < count; i++) children->list[i] = parent->children->list[i];

	children->list[count] = child;
	child->parent = parent;
	_SetPosition(child, &child->position);
	_SetRotation(child, NULL);

	if (parent->children != NULL) mem_free_i(parent->children);
	parent->children = children;
}

void _SetChildren(Transform *parent, Transform **child, int count)
{
	if (parent->children != NULL) mem_free_i(parent->children);
	Children *children = mem_alloc_i(sizeof(Children) + count * sizeof(Transform *));
	children->count = count;

	for (int i = 0; i < count; i++) children->list[i] = child[i];
	parent->children = children;
}

Transform *RotateTransform(Transform *tf, fp32 x, fp32 y, fp32 z)
{
	Rotate(&tf->rotation, x, y, z);
	_SetRotation(tf, NULL);
	return tf;
}

