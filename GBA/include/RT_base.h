#pragma once
#include <hl_math.h>
#include <hl_memory.h>
#include "Transform.h"

//Ԥ����
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

//���߽ṹ��
struct RAY
{
	Vector3 start;
	Vector3 direction;
	int depth;
	fp32 weigth;
};

//������ ray �� t ���ĵ� vt
static inline Vector3 *ray_point(Vector3 *v_out, Ray *ray, fp32 t)
{
	v_out->x = ray->start.x + fp32_mul64(ray->direction.x, t);
	v_out->y = ray->start.y + fp32_mul64(ray->direction.y, t);
	v_out->z = ray->start.z + fp32_mul64(ray->direction.z, t);
	return v_out;
};



//�󽻵���Ϣ
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

//----------------------------��״-----------------------------//

//ָ�����Ͷ���⺯����ָ������
typedef int (*RaycastFC) (Entity *et, Ray *ray, HitPoint *out_hit);

//Transform����ʱ���õĺ�����ָ������
typedef void (*OnTransformUpdateFC) (Entity *et);

//�ƹ� ����ָ������
typedef Color(*LightFC) (Scene *sc, Entity *ptLight, HitPoint *hit);

//λ�ò���
typedef Vector3 *(*GetSamplePositionFC) (Entity *ptLight, int index);

//��״�ķ����ӿ�
typedef struct ShapeInterface
{
	RaycastFC Raycast;
	OnTransformUpdateFC OnTransformUpdate;
	LightFC light;
	GetSamplePositionFC GetSamplePosition;
}ShapeInterface;

//��״�ṹ��
typedef struct BASESHAPE
{
	ShapeInterface *fc;
	Vector3 *samplesPositions;
}BaseShape;

//----------------------------��ͼ-----------------------------//

//ָ���ȡ��ͼ��ɫ������ָ������
typedef Color(*GetColorFC) (Map *map, fp32 u, fp32 v);

//��ͼ�ṹ��
struct MAP { GetColorFC GetColor; };

//----------------------------����-----------------------------//

//ָ����ɫ������ָ������
typedef Color(*ShadeFC) (Scene *sc, HitPoint *hit, Ray *ray);

//�������ʽṹ��
typedef struct MATERIAL { ShadeFC Shade; } Material;

//----------------------------ʵ��-----------------------------//

/*
typedef enum OBJECT_TYPE
{
	ET_Transform,
	ET_Group,
	ET_Entity,
	ET_Light,
}ObjectType;

//����û��ʲô��
typedef struct OBJECT
{
	ObjectType type;
}Object;
*/

//���������Flags
typedef enum ENTITY_PROPERTY
{
	EP_Enable = BIT(0),
	EP_Visable = BIT(1),

	EP_ReceiveSelfShadow = BIT(13),
	EP_ReceiveShadows = BIT(14),
	EP_CastShadows = BIT(15),

	//Ĭ�Ϲ�ѡ������
	EP_Default = 0xFFFFFFFF,
	EP_CanSee = EP_Enable | EP_Visable,
}EntityProperty;

//ʵ��ṹ��
struct ENTITY
{
	EntityProperty property;		//������������
	Transform *transform;			//�任��Ϣָ��
	BaseShape *ptShape;
	Material *ptMaterial;
};

Entity *SetEntity(Entity *et, Transform *transform, BaseShape *ptShape, Material *mt);


//������ṹ��
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
//��Ⱦ�趨*��δ����
typedef struct SETTINGS
{
	u32 maxDepth;
	fp32 threshold;
	int ambientLightSample;
	Color ambientLightColor;
	Color backgroundColor;
}RenderSettings;
*/

//ָ���ȡ�����ɫ������ָ������
typedef Color(*GetSkyColorFC) (Vector3 *direction);

typedef struct Animator Animator;

//����
struct SCENE
{
	int objectCount;		//���������
	int totalCount;		//����ӵƹ������
	Color ambientLightColor;

	GetSkyColorFC GetSkyColor;
	Animator *animator;

	Camera *camera;			//����Ψһ���������û�в���ָ��
	Transform *transforms;	//���ж���ı任��Ϣ
	Entity entities[0];		//Object * count + Light * (totalCount - objCount)
};

//�򵥵Ĵ������������޸ģ�
Scene *CreateScene(int entityCapacity);