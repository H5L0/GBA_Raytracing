#include <hl_memory.h>
#include "RT_base.h"
#include "RT_define.h"
#include "Shape.h"
#include "Material.h"
#include "TextureMap.h"


#include "map_megu.h"
#include "mesh_Teapot.h"
//#include "mesh_test.h"
//#include "mesh_Torus.h"
//#include "mesh_Cube.h"
#include "map_Earth.h"
#include "map_SphereTest.h"
#include "map_SaturnRing.h"
#include "map_Saturn.h"


Scene *GetTestScene()
{
	Scene *sc = CreateScene(8);
	//PrintHexInMode3((u32)sc, 32, 12);
	sc->camera = GetTestCamera();
	GetTestSceneObject(sc);
	GetTestSceneLight(sc);
	sc->GetSkyColor = GetSunSkyColor;
	color_set(&sc->ambientLightColor, fp16_100f(25), fp16_100f(25), fp16_100f(25));
	return sc;
}

Camera *GetTestCamera()
{
	Vector3 pos_cam = { 0, fp32_int(3), fp32_int(-4) };
	//xyz_set(&pos_cam, 0, fp32_int(6), fp32_int(-8));
	//xyz_set(&pos_cam, 0, fp32_int(1), fp32_int(-4));
	//xyz_set(&pos_cam, fp32_100f(-160), fp32_100f(200), fp32_100f(260));
	//xyz_set(&pos_cam, fp32_100f(-160), fp32_100f(200), fp32_100f(-260));
	Vector3 tg = { 0, fp32_int(1), 0 };
	Camera *cam = CreateCamera(&pos_cam, &tg, FP32_1);
	return cam;
}


int GetTestSceneObject(Scene *sc)
{
	void *space = mem_alloc_new(sizeof(MT_Reflect) * 4 + sizeof(Sphere) + sizeof(Box) + sizeof(Plane) + sizeof(Triangle) + sizeof(Mesh));
	MT_Reflect *mt = mem_new(space, sizeof(MT_Reflect) * 4);

	//设定物体位置
	Vector3 pos_sp1 = { fp32_100f(-100), fp32_100f(50), fp32_100f(100) };
	Vector3 pos_sp2 = { fp32_100f(80), fp32_100f(50), fp32_100f(-20) };
	Vector3 pos_3 = { 0, fp32_100f(75), 0 };
	Vector3 pos_plane = { 0, 0, 0 };
	SetTransform(&sc->transforms[0], &pos_sp1, NULL, NULL);
	SetTransform(&sc->transforms[1], &pos_sp2, NULL, NULL);
	SetTransform(&sc->transforms[2], &pos_3, NULL, NULL);
	SetTransform(&sc->transforms[3], &pos_plane, NULL, NULL);
	//设定父子关系
	_AddChild(&sc->transforms[2], &sc->transforms[1]);
	_AddChild(&sc->transforms[1], &sc->transforms[0]);
	//设定材质颜色
	color_setint(&mt[0].color, 1, 0, 0);
	color_setint(&mt[1].color, 0, 0, 1);
	color_setint(&mt[2].color, 0, 1, 0);
	color_set(&mt[3].color, fp16_100f(75), fp16_100f(75), fp16_100f(75));
	//设定材质
	SetMaterial_Reflect(&mt[0], 0, 32, 0, NULL, NULL);
	SetMaterial_Reflect(&mt[1], 0, 64, 0, NULL, NULL);
	SetMaterial_Reflect(&mt[2], 64, 32, 0, NULL, NULL);
	SetMaterial_Reflect(&mt[3], 0, 32, 0, NULL, NULL);
	//SetMaterial_Reflect(&mt[3], 0, 0, 0, NULL, NULL);
	//mt[0].Shade = Shade_Refract;
	//mt[2].Shade = Shade_Refract;

	//设定贴图
	Map_Bitmap *bmp = (Map_Bitmap *)mem_alloc_i(sizeof(Map_Bitmap));
	SetMap_Bitmap(bmp, map_megu_width, map_megu_height, (rgb5 *)map_megu_data);
	//mt[1].map_diffuse = (Map *)bmp;
	mt[2].map_diffuse = (Map *)bmp;
	//棋盘格
	Map_Pattern *ckb = (Map_Pattern *)mem_alloc_i(sizeof(Map_Pattern));
	//mt[3].map_diffuse = SetMap_Pattern(ckb, 4, 4);
	//波纹
	//Map_Wave *wv = (Map_Wave *)mem_alloc_i(sizeof(Map_Wave));
	//mt[3].map_diffuse = SetMap_Wave(wv, 16, 16);
	//Map_Pattern *ckb2 = (Map_Pattern *)mem_alloc_i(sizeof(Map_Pattern));
	//mt[2].map_diffuse = SetMap_Pattern(ckb2, 6, 6);

	//设定形状
	Sphere *sp = mem_new(space, sizeof(Sphere));
	SetShape_Sphere(sp, fp32_100f(50));

	Box *box = mem_new(space, sizeof(Box));
	SetShape_Box(box, fp32_100f(100), fp32_100f(100), fp32_100f(100));

	Triangle *tri = mem_new(space, sizeof(Triangle));
	Vector3 v0, v1, v2;
	xyz_setint(&v0, -1, 1, 0);
	xyz_setint(&v1, 1, 1, 0);
	xyz_setint(&v2, 0, 3, 0);
	SetShape_Triangle(tri, &v0, &v1, &v2);

	Mesh *mesh = mem_new(space, sizeof(Mesh));
	SetShape_Mesh(mesh,
		mesh_Teapot_vertices_count, mesh_Teapot_normals_count, mesh_Teapot_uvs_count, mesh_Teapot_faces_count,
		mesh_Teapot_vertices, mesh_Teapot_normals, mesh_Teapot_uvs, mesh_Teapot_faces);
	//SetShape_Mesh(mesh, mesh_Torus_vertices_count, mesh_Torus_faces_count, mesh_Torus_vertices, NULL, mesh_Torus_faces);
	//SetShape_Mesh(mesh, mesh_test_vertices_count, mesh_test_faces_count, mesh_test_vertices, NULL, mesh_test_faces);
	//SetShape_Mesh(mesh, mesh_Cube_vertices_count, mesh_Cube_faces_count, mesh_Cube_vertices, mesh_Cube_normals, mesh_Cube_uvs, mesh_Cube_faces);

	Plane *pl = mem_new(space, sizeof(Plane));
	SetShape_Plane(pl, fp32_int(-4), fp32_int(-4));

	SetEntity(&sc->entities[0], &sc->transforms[0], (BaseShape *)sp, (Material *)&mt[0]);
	SetEntity(&sc->entities[1], &sc->transforms[1], (BaseShape *)sp, (Material *)&mt[1]);
	SetEntity(&sc->entities[2], &sc->transforms[2], (BaseShape *)box, (Material *)&mt[2]);
	SetEntity(&sc->entities[3], &sc->transforms[3], (BaseShape *)pl, (Material *)&mt[3]);
	//sc->entities[0].property &= ~EP_Visable;
	//sc->entities[1].property &= ~EP_Visable;
	//sc->entities[2].property &= ~EP_Visable;

	sc->objectCount = 4;
	return sc->objectCount;
}

int GetTestSceneLight(Scene *sc)
{
	void *space = mem_alloc_new(sizeof(MT_Light) * 3 + sizeof(Sphere) + sizeof(Box));

	MT_Light *mts = mem_new(space, sizeof(MT_Light) * 3);

	Vector3 pos_0 = { fp32_int(-2), fp32_int(3), fp32_int(-1) };
	Vector3 pos_1 = { fp32_int(2), fp32_int(3), fp32_int(1) };
	Vector3 pos_2 = { fp32_int(1), fp32_int(3), fp32_int(-2) };
	Transform *tfs = &sc->transforms[sc->objectCount];
	SetTransform(&tfs[0], &pos_0, NULL, NULL);
	SetTransform(&tfs[1], &pos_1, NULL, NULL);
	SetTransform(&tfs[2], &pos_2, NULL, NULL);
	Vector3 direction = { -FP32_1, -FP32_1, -FP32_1 };
	SetRotationForward(&tfs[1].rotation, xyz_normalize(&direction));
	//设定灯光材质
	SetMaterial_Light(&mts[0], 100, 100, 100, 8);
	SetMaterial_Light(&mts[1], 100, 100, 100, 8);
	SetMaterial_Light(&mts[2], 100, 100, 100, 4);
	Sphere *sp = mem_new(space, sizeof(Sphere));
	SetShape_Sphere(sp, fp32_100f(50));
	Box *box = mem_new(space, sizeof(Box));
	SetShape_Box(box, FP32_1, FP32_1, FP32_1);

	Entity *lts = &sc->entities[sc->objectCount];
	SetEntity(&lts[0], &tfs[0], (BaseShape *)sp, (Material *)&mts[0]);
	SetEntity(&lts[1], &tfs[1], (BaseShape *)sp, (Material *)&mts[1]);
	SetEntity(&lts[2], &tfs[2], (BaseShape *)sp, (Material *)&mts[2]);
	lts[0].property &= ~EP_Enable;
	lts[2].property &= ~EP_Enable;
	//设定阴影
	mts[1].samples = 1;
	sc->totalCount = sc->objectCount + 3;
	return sc->totalCount - sc->objectCount;
}




//一个宇宙场景
Scene *GetEarthScene()
{
	Scene *sc = CreateScene(3);
	void *space = mem_alloc_new(4 * 64);
	//设置摄像机
	Vector3 pos_cam;
	xyz_setint(&pos_cam, 0, 4, -3);
	Vector3 tg = { 0, 0, 0 };
	sc->camera = CreateCamera(&pos_cam, &tg, FP32_1);

	//设定物体位置
	Vector3 pos_earth = { 0, 0, 0 };
	Vector3 pos_atm = { 0, 0, 0 };
	Vector3 pos_sun = { fp32_int(-16), 0, 0 };
	SetTransform(&sc->transforms[0], &pos_earth, NULL, NULL);
	SetTransform(&sc->transforms[1], &pos_atm, NULL, NULL);
	SetTransform(&sc->transforms[2], &pos_sun, NULL, NULL);
	//设定物体大小
	Sphere *sp = mem_new(space, sizeof(Sphere) * 3);
	SetShape_Sphere(&sp[0], fp32_100f(100));
	SetShape_Sphere(&sp[1], fp32_100f(108));
	SetShape_Sphere(&sp[2], fp32_100f(100));
	//地球材质
	MT_Reflect *mt_earth = mem_new(space, sizeof(MT_Reflect));
	color_set(&mt_earth->color, fp16_100f(40), fp16_100f(30), fp16_100f(20));
	Map_Bitmap *bmp = mem_new(space, sizeof(Map_Bitmap));
	SetMap_Globe(bmp, map_Earth_width, map_Earth_height, (rgb5 *)map_Earth_data);
	//Map_Globe *bmp = mem_new(space, sizeof(Map_Globe));
	//SetMap_Globe(bmp, map_SphereTest_width, map_SphereTest_height, (rgb5 *)map_SphereTest_data);
	//Map *ckb = SetMap_Pattern(mem_alloc_i(sizeof(Map_Pattern)), 4, 4);
	SetMaterial_Reflect(mt_earth, 0, 0, 0, (Map *)bmp, NULL);

	//大气层材质
	MT_Transparent *mt_atm = mem_new(space, sizeof(MT_Transparent));
	SetMaterial_Transparent(mt_atm, 60, 90, 240, 40, NULL);
	mt_atm->Shade = Shade_Glory;
	//太阳材质（灯光）
	MT_Light *mt_sun = mem_new(space, sizeof(MT_Light));
	SetMaterial_Light(mt_sun, 100, 100, 100, 512);

	SetEntity(&sc->entities[0], &sc->transforms[0], (BaseShape *)&sp[0], (Material *)mt_earth);
	SetEntity(&sc->entities[1], &sc->transforms[1], (BaseShape *)&sp[1], (Material *)mt_atm);
	SetEntity(&sc->entities[2], &sc->transforms[2], (BaseShape *)&sp[2], (Material *)mt_sun);
	//大气层不投射阴影（或地球不接受阴影）
	//sc->entities[1].property &= ~EP_CastShadows;
	sc->entities[0].property &= ~EP_ReceiveShadows;
	//sc->entities[0].property &= ~EP_Enable;
	sc->objectCount = 2;
	sc->totalCount = 3;
	//sc->GetSkyColor = GetGalaxyColor;
	sc->GetSkyColor = GetBlackBackground;
	fp16 ao = fp16_100f(5);
	color_set(&sc->ambientLightColor, ao, ao, ao);
	return sc;
}


Scene *GetSaturnScene()
{
	Scene *sc = CreateScene(3);
	void *space = mem_alloc_new(4 * 64);
	//设置摄像机
	Vector3 pos_cam;
	xyz_setint(&pos_cam, 0, 1, -3);
	Vector3 tg = { 0, 0, 0 };
	sc->camera = CreateCamera(&pos_cam, &tg, FP32_1);

	//设定物体位置
	Vector3 pos_saturn = { 0, 0, 0 };
	Vector3 pos_ring = { 0, 0, 0 };
	Vector3 pos_sun = { fp32_int(-16), fp32_int(8), 0 };
	SetTransform(&sc->transforms[0], &pos_saturn, NULL, NULL);
	SetTransform(&sc->transforms[1], &pos_ring, NULL, NULL);
	SetTransform(&sc->transforms[2], &pos_sun, NULL, NULL);
	//设定物体大小
	Sphere *sp_saturn = mem_new(space, sizeof(Sphere));
	Plane *sp_ring = mem_new(space, sizeof(Plane));
	Sphere *sp_sun = mem_new(space, sizeof(Sphere));
	SetShape_Sphere(sp_saturn, fp32_100f(100));
	sp_saturn->base.fc->Raycast = Raycast_Sphere_V;
	SetShape_Plane(sp_ring, fp32_100f(128), fp32_100f(220));
	sp_ring->base.fc->Raycast = Raycast_Ring;	//修改圆环的投射函数
	SetShape_Sphere(sp_sun, fp32_100f(100));

	//土星材质
	MT_Reflect *mt_saturn = mem_new(space, sizeof(MT_Reflect));
	color_set(&mt_saturn->color, fp16_100f(40), fp16_100f(30), fp16_100f(20));
	Map_Bitmap *map_saturn = mem_new(space, sizeof(Map_Bitmap));
	SetMap_Bitmap(map_saturn, map_Saturn_width, map_Saturn_height, (rgb5 *)map_Saturn_data);
	SetMaterial_Reflect(mt_saturn, 0, 0, 0, (Map *)map_saturn, NULL);
	//圆环
	MT_Transparent *mt_ring = mem_new(space, sizeof(MT_Transparent));
	Map_Bitmap *map_ring = mem_new(space, sizeof(Map_Bitmap));
	SetMap_Bitmap(map_ring, map_SaturnRing_width, map_SaturnRing_height, (rgb5 *)map_SaturnRing_data);
	SetMaterial_Transparent(mt_ring, 130, 110, 80, 100, (Map *)map_ring);
	//SetMaterial_Reflect(mt_ring, 0, 0, 0, NULL, NULL);
	//太阳材质（灯光）
	MT_Light *mt_sun = mem_new(space, sizeof(MT_Light));
	SetMaterial_Light(mt_sun, 100, 100, 100, 256);

	SetEntity(&sc->entities[0], &sc->transforms[0], (BaseShape *)sp_saturn, (Material *)mt_saturn);
	SetEntity(&sc->entities[1], &sc->transforms[1], (BaseShape *)sp_ring, (Material *)mt_ring);
	SetEntity(&sc->entities[2], &sc->transforms[2], (BaseShape *)sp_sun, (Material *)mt_sun);
	//大气层不投射阴影（或地球不接受阴影）
	//sc->entities[1].property &= ~EP_CastShadows;
	//sc->entities[0].property &= ~EP_Enable;
	sc->objectCount = 2;
	sc->totalCount = 3;
	sc->GetSkyColor = GetBlackBackground;
	fp16 ao = fp16_100f(5);
	color_set(&sc->ambientLightColor, ao, ao, ao);
	return sc;
}

Color GetSunSkyColor(Vector3 *dir)
{
	Color c = { 0, 0, 0 };
	//c.x = 512 + fp16_mul32(-512, f);
	//c.y = 1000 + fp16_mul32(-512, f);
	//c.z = 1000 + fp16_mul32(23, f);
	Color orange = { fp16_100f(100), fp16_100f(60), fp16_100f(28) };
	Color lightBlue = { fp16_100f(25), fp16_100f(70), fp16_100f(70) };
	Color deepBlue = { fp16_100f(20), fp16_100f(40), fp16_100f(95) };
	fp32 r0 = 0;
	fp32 r1 = fp32_100f(20);
	fp32 r2 = fp32_100f(95);
	fp16 t;

	//if (dir->y < r0) return c;
	if (dir->y < r1)
	{
		t = fp16_fp32(fp32_div32(dir->y - r0, r1 - r0));
		color_lerp(&c, &orange, &lightBlue, t);
	}
	else if (dir->y < r2)
	{
		t = fp16_fp32(fp32_div32(dir->y - r1, r2 - r1));
		color_lerp(&c, &lightBlue, &deepBlue, t);
	}
	else return deepBlue;
	return c;
}

Color GetGalaxyColor(Vector3 *dir)
{
	fp32 x = dir->x >> 5;
	fp32 y = dir->y >> 5;
	Color c = { FP16_1, FP16_1, FP16_1 };
	if ((((u32)x & 1111) == 0) && (((u32)y & 1111) == 0)) return c;
	color_set(&c, 0, 0, 0);
	return c;
}

Color GetSpaceColor(Vector3 *dir)
{
	Color c = { 0, 0, 0 };
	color16_rgb5(RGB5(1, 2, 2));
	Color lightBlue = color16_rgb5(RGB5(1, 2, 2));
	Color deepBlue = color16_rgb5(RGB5(0, 1, 2));

	fp32 r1 = fp32_100f(-45);
	fp32 r2 = fp32_100f(45);
	fp16 t;

	if (dir->y < r1)
	{
		return lightBlue;
	}
	else if (dir->y < r2)
	{
		t = fp16_fp32(fp32_div32(dir->y - r1, r2 - r1));
		color_lerp(&c, &lightBlue, &deepBlue, t);
		return c;
	}
	else return deepBlue;
}