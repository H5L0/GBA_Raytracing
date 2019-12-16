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


//�趨��������Position
//���pos==NULL,��ֻ���¾ֲ�����LocalPosition
void _SetPosition(Transform *transform, Vector3 *pos)
{
	//�趨��������
	if (pos == NULL) pos = &transform->position;
	else transform->position = *pos;

	//���±�������
	if (transform->parent == NULL) transform->localPosition = *pos;
	else
	{
		TransformPointToLocal(&transform->parent->rotation, &transform->parent->position, pos, &transform->localPosition);
	}

	if (transform->entity != NULL)
		transform->entity->ptShape->fc->OnTransformUpdate(transform->entity);

	//�����Ӷ�����������
	if (transform->children != NULL)
		_UpdateChildrenPosition(transform);
}

//�趨�ֲ�����LocalPosition
//���pos==NULL,��ֻ������������Position
void _SetLocalPosition(Transform *transform, Vector3 *pos)
{
	//�趨�ֲ�����
	if (pos == NULL) pos = &transform->localPosition;
	else transform->localPosition = *pos;

	//������������
	if (transform->parent == NULL) transform->position = *pos;
	else
	{
		//�Ӿֲ�����ת������������,�ֲ������Դ������������תΪ������(Rotation)
		TransformPointToWorld(&transform->parent->rotation, &transform->parent->position, pos, &transform->position);
	}

	if (transform->entity != NULL)
		transform->entity->ptShape->fc->OnTransformUpdate(transform->entity);

	//�����Ӷ�����������
	if (transform->children != NULL)
		_UpdateChildrenPosition(transform);
}

//�����Ӷ������������
void _UpdateChildrenPosition(Transform *parent)
{
	/*
	//�����Ӷ�����������
	int count = parent->children->count;
	Transform *child;
	Vector3 temp;
	for (int i = 0; i < count; i++)
	{
		child = parent->children->list[i];

		//�趨��������
		//TransformToWorld(&parent->rotation, &child->localPosition, &temp);
		//xyz_add(&child->position, &parent->position, &temp);

		//if (child->entity != NULL)
		//	child->entity->ptShape->fc->OnTransformUpdate(child->entity);

		//if (child->children != NULL) _UpdateChildrenPosition(child);
	}*/

	int count = parent->children->count;
	for (int i = 0; i < count; i++)
	{
		//�趨�Ӷ������꣨�ݹ飩����Ҫ����local
		_SetLocalPosition(parent->children->list[i], NULL);
	}
}


//�趨������ת������ϵ
void _SetRotation(Transform *transform, Rotation *rot)
{
	if (rot == NULL) rot = &transform->rotation;
	else transform->rotation = *rot;

	//���±��ػ�����ϵ
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
		//ֻҪ������ת�����仯����Ҫ�����Ӷ����Position
	}
}

//�趨������ת������ϵ
//������ת����ϵ������ ������������ת���� Ϊ��������ϵ
void _SetLocalRotation(Transform *transform, Rotation *rot)
{
	if (rot == NULL) rot = &transform->localRotation;
	else transform->localRotation = *rot;

	//�������������ϵ
	if (transform->parent == NULL) transform->rotation = *rot;
	else
	{
		TransformMatrixToWorld(&transform->parent->rotation, rot, &transform->rotation);
	}

	if (transform->children != NULL)
	{
		_UpdateChildrenRotation(transform);
		_UpdateChildrenPosition(transform);
		//ֻҪ������ת�����仯����Ҫ�����Ӷ����Position
	}
}


void _UpdateChildrenRotation(Transform *parent)
{
	int count = parent->children->count;
	for (int i = 0; i < count; i++)
	{
		//ֻ��Ҫ�����Ӷ�������������ϵ
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
	//����children������
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

