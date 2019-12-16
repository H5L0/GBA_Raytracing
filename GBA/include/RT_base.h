#pragma once
#include <hl_math.h>
#include <hl_memory.h>
#include "Transform.h"

//预声明
typedef struct RAY Ray;
typedef struct HITPOINT HitPoint;
typedef struct BASESHAPE BaseShape;
typedef struct MATERIAL Material;
typedef struct MAP Map;
typedef struct TRANSFORM Transform;
typedef struct CHILDREN Children;
typedef struct ENTITY Entity;
typedef struct ENTITY Light;
typedef struct SCENE Scene;
typedef struct CAMERA Camera;

//射线结构体
struct RAY
{
	Vector3 start;
	Vector3 direction;
	int depth;
	fp32 weigth;
};

//求射线 ray 上 t 处的点 vt
static inline Vector3 *ray_point(Vector3 *v_out, Ray *ray, fp32 t)
{
	v_out->x = ray->start.x + fp32_mul64(ray->direction.x, t);
	v_out->y = ray->start.y + fp32_mul64(ray->direction.y, t);
	v_out->z = ray->start.z + fp32_mul64(ray->direction.z, t);
	return v_out;
};



//求交点信息
struct HITPOINT
{
	Entity *et;
	Vector3 position, normal;
	fp32 distance;
	fp32 u, v;
	Bool quickCheck;
	Vector3 tempVector;
	Color debugColor;
};


#define OFFSET_SHIFT 10
static inline xyz *OffsetHitPoint(Vector3 *v_out, HitPoint *hit)
{
	v_out->x = hit->position.x + (fp32_mul32(hit->normal.x, hit->distance) >> OFFSET_SHIFT);
	v_out->y = hit->position.y + (fp32_mul32(hit->normal.y, hit->distance) >> OFFSET_SHIFT);
	v_out->z = hit->position.z + (fp32_mul32(hit->normal.z, hit->distance) >> OFFSET_SHIFT);
	return v_out;
}

static inline xyz *OffsetHitPoint_Reverse(Vector3 *v_out, HitPoint *hit)
{
	v_out->x = hit->position.x - (fp32_mul32(hit->normal.x, hit->distance) >> OFFSET_SHIFT);
	v_out->y = hit->position.y - (fp32_mul32(hit->normal.y, hit->distance) >> OFFSET_SHIFT);
	v_out->z = hit->position.z - (fp32_mul32(hit->normal.z, hit->distance) >> OFFSET_SHIFT);
	return v_out;
}

//----------------------------形状-----------------------------//

//指向光线投射检测函数的指针类型
typedef int (*RaycastFC) (Entity *et, Ray *ray, HitPoint *out_hit);

//Transform更新时调用的函数的指针类型
typedef void (*OnTransformUpdateFC) (Entity *et);

//灯光 函数指针类型
typedef Color(*LightFC) (Scene *sc, Entity *ptLight, HitPoint *hit);

//位置采样
typedef Vector3 *(*GetSamplePositionFC) (Entity *ptLight, int index);

//形状的方法接口
typedef struct ShapeInterface
{
	RaycastFC Raycast;
	OnTransformUpdateFC OnTransformUpdate;
	LightFC light;
	GetSamplePositionFC GetSamplePosition;
}ShapeInterface;

//形状结构体
typedef struct BASESHAPE
{
	ShapeInterface *fc;
	Vector3 *samplesPositions;
}BaseShape;

//----------------------------贴图-----------------------------//

//指向获取贴图颜色函数的指针类型
typedef Color(*GetColorFC) (Map *map, fp32 u, fp32 v);

//贴图结构体
struct MAP { GetColorFC GetColor; };

//----------------------------材质-----------------------------//

//指向着色函数的指针类型
typedef Color(*ShadeFC) (Scene *sc, HitPoint *hit, Ray *ray);

//基础材质结构体
typedef struct MATERIAL { ShadeFC Shade; } Material;

//----------------------------实体-----------------------------//

/*
typedef enum OBJECT_TYPE
{
	ET_Transform,
	ET_Group,
	ET_Entity,
	ET_Light,
}ObjectType;

//估计没有什么用
typedef struct OBJECT
{
	ObjectType type;
}Object;
*/

//物体的属性Flags
typedef enum ENTITY_PROPERTY
{
	EP_Enable = BIT(0),
	EP_Visable = BIT(1),

	EP_ReceiveSelfShadow = BIT(13),
	EP_ReceiveShadows = BIT(14),
	EP_CastShadows = BIT(15),

	//默认勾选的属性
	EP_Default = 0xFFFFFFFF,
	EP_CanSee = EP_Enable | EP_Visable,
}EntityProperty;

//实体结构体
struct ENTITY
{
	EntityProperty property;		//物体的许多属性
	Transform *transform;			//变换信息指针
	BaseShape *ptShape;
	Material *ptMaterial;
};

Entity *SetEntity(Entity *et, Transform *transform, BaseShape *ptShape, Material *mt);


//摄像机结构体
struct CAMERA
{
	int property;
	Transform *transform;
	//Vector3 *position;
	//Rotation *rotation;
	fp32 size_x, size_y;
};

Camera *CreateCamera(Vector3 *pos, Vector3 *target, fp32 size);
//void InitCamera(Camera *cam, Vector3 *pos, Rotation *rot, fp32 size);
Ray *ScreenToRay(Camera *cam, Ray *ray, int x, int y);
void SetCameraTarget(Camera *cam, Vector3 *target);

/*
//渲染设定*暂未采用
typedef struct SETTINGS
{
	u32 maxDepth;
	fp32 threshold;
	int ambientLightSample;
	Color ambientLightColor;
	Color backgroundColor;
}RenderSettings;
*/

//指向获取天空颜色函数的指针类型
typedef Color(*GetSkyColorFC) (Vector3 *direction);

typedef struct Animator Animator;

//场景
struct SCENE
{
	int objectCount;		//物体的数量
	int totalCount;		//物体加灯光的数量
	Color ambientLightColor;

	GetSkyColorFC GetSkyColor;
	Animator *animator;

	Camera *camera;			//场景唯一的摄像机，没有采用指针
	Transform *transforms;	//所有对象的变换信息
	Entity entities[0];		//Object * count + Light * (totalCount - objCount)
};

//简单的创建场景（待修改）
Scene *CreateScene(int entityCapacity);