#pragma once
#include <hl_memory.h>
#include "RT_base.h"

typedef struct RAY Ray;
typedef struct BASESHAPE BaseShape;
typedef struct _AABB AABB;
typedef struct BOUNDINGSPHERE BSP;
typedef struct TRANSFORM Transform;
typedef struct ENTITY Entity;
typedef struct HITPOINT HitPoint;


//Bool Raycast_null(Entity *et, Ray *ray, HitPoint *out_hit) { return FALSE; }
//void OnTransformUpdate_null(Entity *et) {}
//void Light_null(Entity *et){}


#define ShapeInterfaceCode(Shape)									\
Bool Raycast_##Shape(Entity *et, Ray *ray, HitPoint *hit);		\
void OnTransformUpdate_##Shape(Entity *et);						\
Color Lit_##Shape(Scene *sc, Entity *lt, HitPoint *hit);			\
static ShapeInterface ShapeInterface_##Shape =					\
{																		\
	Raycast_##Shape,													\
	OnTransformUpdate_##Shape,										\
	Lit_##Shape,														\
};																		\


//��״�ṹ�壺Բ��
typedef struct SPHERE
{
	BaseShape base;
	fp32 radius;
}Sphere;

ShapeInterfaceCode(Sphere)


//��״�ṹ�壺ƽ��
typedef struct PLANE
{
	BaseShape base;
	fp32 x; fp32 z;
}Plane;

ShapeInterfaceCode(Plane)


//��״�ṹ�壺������
typedef struct BOX
{
	BaseShape base;
	fp32 x; fp32 y; fp32 z;
}Box;

ShapeInterfaceCode(Box)


//��״�ṹ�壺������
typedef struct TRIANGLE
{
	BaseShape base;
	Vector3 v0; Vector3 v1; Vector3 v2;
}Triangle;

ShapeInterfaceCode(Triangle)


typedef struct UNIT2 { u16 u; u16 v; }Unit2;

//��״�ṹ�壺����������
typedef struct MESH
{
	BaseShape base;
	u16 verticesCount;		//���㣬���65536 **û���õı���
	u16 trianglesCount;		//����������
	u16 normalsCount;
	u16 uvsCount;

	Vector3 *vertices;		//��������
	Vector3 *normals;		//���㷨�ߣ�ƽ��/��ƽ����/�淨�ߣ���Ч��ƽ����
	Unit2 *uvs;

	u16 *triangles;			//�������Ӧ�Ķ���������3�����㣬3����ͼ���꣬3������
	u16 *normalIndex;		//���ΪNULL,Ӳ��
	u16 *uvIndex;				//���ΪNULL,ֱ�Ӱ�˳���uv��

	Vector3 *worldVertices;
	Vector3 *worldNormals;

	AABB *ptAABB;
	BSP *ptBS;
}Mesh;

ShapeInterfaceCode(Mesh)


//��״�ṹ�壺AABB������Χ��
typedef struct _AABB
{
	//AABBû��ʹ��Raycastָ��
	Vector3 position;
	Vector3 size;
	Vector3 max;
	Vector3 min;
}AABB;

typedef struct BOUNDINGSPHERE
{
	Vector3 position;
	fp32 radius;
}BSP;

int Raycast_AABB(AABB *aabb, Ray *ray);
int Raycast_BSP(BSP *bsp, Ray *ray);
//static inline BSP *SetBSP_Mesh(BSP *bsp, Mesh mesh);

//int Raycast_Sphere(Entity *et, Ray *ray, HitPoint *hit);
int Raycast_Sphere_V(Entity *et, Ray *ray, HitPoint *hit);
//int Raycast_Plane(Entity *et, Ray *ray, HitPoint *hit);
int Raycast_Ring(Entity *et, Ray *ray, HitPoint *hit);
//int Raycast_Box(Entity *et, Ray *ray, HitPoint *hit);
//int Raycast_Triangle(Entity *et, Ray *ray, HitPoint *hit);
//int Raycast_Mesh(Entity *et, Ray *ray, HitPoint *hit);


static inline Sphere *SetShape_Sphere(Sphere *sp, fp32 radius)
{
	sp->base.fc = &ShapeInterface_Sphere;
	sp->radius = radius;
	return sp;
}

static inline Box *SetShape_Box(Box *box, fp32 x, fp32 y, fp32 z)
{
	box->base.fc = &ShapeInterface_Box;
	box->x = x; box->y = y; box->z = z;
	return box;
}

static inline Plane *SetShape_Plane(Plane *pl, fp32 x, fp32 z)
{
	pl->base.fc = &ShapeInterface_Plane;
	pl->x = x; pl->z = z;
	return pl;
}

static inline BaseShape *SetShape_Triangle(Triangle *tri, Vector3 *v0, Vector3 *v1, Vector3 *v2)
{
	tri->base.fc = &ShapeInterface_Triangle;
	tri->v0 = *v0;
	tri->v1 = *v1;
	tri->v2 = *v2;
	return (BaseShape *)tri;
}

static inline BaseShape *SetShape_Mesh(Mesh *ms,
	u16 verticesCount, u16 normalsCount, u16 uvsCount, u16 trianglesCount,
	Vector3 *vertices, Vector3 *normals, Unit2 *uvs, u16 *triangles)
{
	ms->base.fc = &ShapeInterface_Mesh;
	ms->verticesCount = verticesCount;
	ms->normalsCount = normalsCount;
	ms->uvsCount = uvsCount;
	ms->trianglesCount = trianglesCount;
	ms->vertices = vertices;
	//**ֱ���趨normals
	//���smooth��normals��3�����㷨�߲�ֵ��triangles 9��/��
	//����normalsΪ�淨�ߣ�triangles 6��/��  (9?)
	//ms->smooth = TRUE;
	/*//���ȱʧ���ߣ����㲹��
	if (normals == NULL)
	{
		ms->smooth = FALSE;
		normals = mem_alloc_i(sizeof(Vector3) * trianglesCount);
		Vector3 E1, E2;
		Vector3 *v0, *v1, *v2;
		for (int j = 0; j < trianglesCount; j++)
		{
			v0 = &vertices[triangles[j * 9]];
			v1 = &vertices[triangles[j * 9 + 1]];
			v2 = &vertices[triangles[j * 9 + 2]];
			xyz_normalize(xyz_cross(&normals[j], xyz_sub(&E2, v2, v0), xyz_sub(&E1, v1, v0)));
		}
	}
	else ms->smooth = TRUE;
	*/
	ms->normals = normals;
	ms->uvs = uvs;
	ms->triangles = triangles;

	//**�������������ʱ����Ҫ�������񶥵�����
	ms->worldVertices = (Vector3 *)mem_alloc_i(ms->verticesCount * sizeof(Vector3));
	ms->worldNormals = (Vector3 *)mem_alloc_i(ms->normalsCount * sizeof(Vector3));

	ms->ptBS = NULL;

	return (BaseShape *)ms;
}
