#pragma once
#include "hl_math.h"
#include "RT_base.h"

//基础节点
typedef union BNODE
{
	//分支节点
	struct
	{
		Vector3 pos;
		Vector3 size;

		BNODE *nodeA;
		BNODE *nodeB;
	};

	//叶节点
	struct
	{
		Vector3 pos;
		Vector3 size;

		Entity *entity;

	};

}BNode;


//计算长方体表面积 **不乘2
static inline fp32 GetSurfaceArea(Vector3 *size)
{
	return (fp32_mul64(size->x, size->y) + fp32_mul64(size->x, size->z) + fp32_mul64(size->y, size->z)) * 2;
}

//计算两长方体相交体积
static inline fp32 GetOverlapVolume(Vector3 *pos1, Vector3 *size1, Vector3 *pos2, Vector3 *size2)
{

}


BNode GetEntityBox(Entity *et)
{
	BNode leaf;
	leaf.pos = *et->position;
	leaf.size = et->rotation->forward;
	leaf.entity = et;
	return leaf;
}


//建立BVH树
void BuildBVH(Scene *scene)
{

}


void DrawBox(Camera *cam, Vector3 *pos, Vector3 *size)
{
	Vector3 vPos[8];
	unsigned char ix[8];
	unsigned char iy[8];
	int x, y;
	Vector3 vc;
	for (int i = 0; i < 8; i++)
	{
		vc = *size;
		for (int j = 0; j < 3; j++)
		{ if (i & BIT(j))  (&vc.x)[j] *= -1; }
		xyz_sub(&vc, xyz_add(&vc, pos, &vc), cam->position);
		TransformTo(cam->rotation, &vc, &vPos[i]);
		x = (vPos[i].x >> (FP32_FBIT - 6)) + SCREEN_X / 2;
		y = (vPos[i].y >> (FP32_FBIT - 6)) + SCREEN_Y / 2;
		ix[i] = x < 0 ? 0 : (x > SCREEN_X - 1 ? SCREEN_X - 1 : x);
		iy[i] = y < 0 ? 0 : (y > SCREEN_Y - 1 ? SCREEN_Y - 1 : y);
	}

	rgb5 color = RGB5_RED;

	DrawLine(ix[0], iy[0], ix[1], iy[1], color);
	DrawLine(ix[0], iy[0], ix[2], iy[2], color);
	DrawLine(ix[0], iy[0], ix[4], iy[4], color);

	DrawLine(ix[5], iy[5], ix[1], iy[1], color);
	DrawLine(ix[5], iy[5], ix[4], iy[4], color);
	DrawLine(ix[5], iy[5], ix[7], iy[7], color);

	DrawLine(ix[3], iy[3], ix[1], iy[1], color);
	DrawLine(ix[3], iy[3], ix[2], iy[2], color);
	DrawLine(ix[3], iy[3], ix[7], iy[7], color);

	DrawLine(ix[6], iy[6], ix[2], iy[2], color);
	DrawLine(ix[6], iy[6], ix[4], iy[4], color);
	DrawLine(ix[6], iy[6], ix[7], iy[7], color);
}