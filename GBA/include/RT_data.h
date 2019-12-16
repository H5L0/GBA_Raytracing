#pragma once
#include "RT_base.h"
#include "Shape.h"
#include "Material.h"
#include "TextureMap.h"

//需要确保枚举4字节
enum DataType
{
	D_Shape_Sphere = 0xA000,
	D_Shape_Plane = 0xA001,
	D_Shape_Box = 0xA002,
	D_Shape_Mesh = 0xA003,

	D_Material_Light = 0xB000,
	D_Material_Reflect = 0xB001,
};

typedef struct
{
	enum DataType dataType;
	int offset;
}DataIdentity;

typedef struct
{
	int objectAmount;
	int totalAmount;
	int TransformAmount;
	int childrenAmount;
	int dataCount;
	//int requireDataSize;

	char data[0];
	//entitiesData*n
	//TransformData*n
	//childrenData*x
	//DataInfo*n
	//Datas...
}SceneData;

typedef struct
{
	//char name[32];
	EntityProperty property;
	int transformID;
	//int shapeType;
	int shapeID;
	int materialID;
}EntityData;

typedef struct
{
	Vector3 position;
	Vector3 right;
	Vector3 up;
	int childCount;
}TransformData;

typedef int ChildrenID;


typedef struct{ fp32 radius; }SphereData;
typedef struct{ Vector3 size; }BoxData;
typedef struct{ fp32 x; fp32 z; }PlaneData;
typedef struct
{
	u16 verticesAmount;
	u16 triangleAmount;
	u16 normalAmount;
	u16 uvAmount;

	char data[0];
}MeshData;

static inline Sphere *Deserialize_Sphere(SphereData *data)
{
	Sphere *sp = mem_alloc_i(sizeof(Sphere));
	return SetShape_Sphere(sp, data->radius);
}

static inline Box *Deserialize_Box(BoxData *data)
{
	Box *box = mem_alloc_i(sizeof(Box));
	return SetShape_Box(box, data->size.x, data -> size.y, data->size.z);
}

static inline Plane *Deserialize_Plane(PlaneData *data)
{
	Plane *plane = mem_alloc_i(sizeof(Plane));
	return SetShape_Plane(plane, data->x, data->z);
	return plane;
}

static inline int _Deserialize_Mesh(MeshData *data, Mesh *mesh)
{
	mesh->base.fc = &ShapeInterface_Mesh;
	mesh->verticesCount = data->verticesAmount;
	mesh->trianglesCount = data->triangleAmount;
	mesh->normalsCount = data->normalAmount;
	mesh->uvsCount = data->uvAmount;

	mesh->vertices = (Vector3 *)&data->data;
	mesh->triangles = (u16 *)&mesh->vertices[mesh->verticesCount];
	//想起来可以省去1fp32
	mesh->normals = (Vector3 *)&mesh->triangles[mesh->trianglesCount * 3];
	mesh->uvs = (Unit2 *)&mesh->normals[mesh->normalsCount];

	//**更新物体坐标的时候需要更新网格顶点坐标
	mesh->worldVertices = (Vector3 *)mem_alloc_i(mesh->verticesCount * sizeof(Vector3));
	mesh->worldNormals = (Vector3 *)mem_alloc_i(mesh->normalsCount * sizeof(Vector3));

	//mesh->smooth = TRUE;
	mesh->ptBS = NULL;

	return sizeof(Mesh) + sizeof(Vector3) * (mesh->verticesCount + mesh->normalsCount) +
		sizeof(Unit2) * mesh->uvsCount + sizeof(u16) * mesh->trianglesCount * 9;
}


static inline Mesh *Deserialize_Mesh(MeshData *data)
{
	int size = sizeof(Mesh) + sizeof(Vector3) * (data->verticesAmount + data->normalAmount);
	//+ sizeof(Unit2) * data->uvAmount + sizeof(u16) * data->triangleAmount * 3;
	//Mesh *mesh = mem_alloc_i(size);
	Mesh *mesh = mem_alloc_e(size);

	mesh->base.fc = &ShapeInterface_Mesh;
	mesh->verticesCount = data->verticesAmount;
	mesh->trianglesCount = data->triangleAmount;
	mesh->normalsCount = data->normalAmount;
	mesh->uvsCount = data->uvAmount;

	mesh->vertices = (Vector3 *)&data->data;
	mesh->triangles = (u16 *)&mesh->vertices[mesh->verticesCount];
	//想起来可以省去1fp32
	mesh->normals = (Vector3 *)&mesh->triangles[mesh->trianglesCount * 3];
	mesh->uvs = (Unit2 *)&mesh->normals[mesh->normalsCount];

	//**更新物体坐标的时候需要更新网格顶点坐标
	mesh->worldVertices = (Vector3 *)((char *)mesh + sizeof(Mesh));
	mesh->worldNormals = &mesh->worldVertices[mesh->verticesCount];

	//mesh->smooth = TRUE;
	//mesh->ptBS = NULL;
	mesh->ptBS = mem_alloc_i(sizeof(BSP));
	return mesh;
}


//通用的反射材质
typedef struct MATERIAL_DATA_REFLECT
{
	Color color;
	fp16 metallic;		//金属度
	fp16 reflectance;	//反射率
	fp16 roughness;		//粗糙度
	int map_diffuse;		//漫反射贴图
	int map_reflect;		//反射贴图
}MTData_Reflect;


static inline MT_Reflect *Deserialize_MT_Reflect(MTData_Reflect *data)
{
	MT_Reflect *mt = mem_alloc_i(sizeof(MT_Reflect));
	mt->Shade = Shade_Reflect;
	mt->color = data->color;
	mt->metallic = data->metallic;
	mt->reflectance = data->reflectance;
	mt->roughness = data->roughness;
	mt->map_diffuse = NULL;
	mt->map_reflect = NULL;
	return mt;
}

typedef struct MATERIAL_DATA_LIGHT
{
	Color color;
	u16 intensity;
}MTData_Light;

static inline MT_Light *Deserialize_MT_Light(MTData_Light *data)
{
	MT_Light *mt = mem_alloc_i(sizeof(MT_Light));
	mt->Shade = Shade_Light;
	mt->color = data->color;
	mt->intensity = data->intensity * 2;
	//***
	mt->samples = 0;
	return mt;
}

typedef struct
{
	int ID;
	int width;
	int height;
	//int bitPerPixel;
	//int length;	//数据总长
	char data[0];
}TextureData;


static inline Map *Deserialize_Map(TextureData *data)
{
	Map_Bitmap *map = mem_alloc_i(sizeof(Map_Bitmap));
	return SetMap_Bitmap(map, data->width, data->height, (rgb5*)&data->data);
}


//从只读内存加载场景
Scene *LoadScene(SceneData *sceneData);