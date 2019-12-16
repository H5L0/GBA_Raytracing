#include "RT_base.h"
#include "RT_define.h"
#include "RT_data.h"


void *DeserializeData(DataIdentity *dataArrange, int id)
{
	DataIdentity dataInfo = dataArrange[id];
	//数据实际位置为 数据排列信息的起始位置 + offset
	void *dataPosition = (void *)((int)dataArrange + dataInfo.offset);

	switch (dataInfo.dataType)
	{
	case D_Shape_Sphere:
		return Deserialize_Sphere(dataPosition);
	case D_Shape_Plane:
		return Deserialize_Plane(dataPosition);
	case D_Shape_Box:
		return Deserialize_Box(dataPosition);
	case D_Shape_Mesh:
		return Deserialize_Mesh(dataPosition);
	case D_Material_Light:
		return Deserialize_MT_Light(dataPosition);
	case D_Material_Reflect:
		return Deserialize_MT_Reflect(dataPosition);
	default:
		return NULL;
	}
}

Transform *LoadTransforms(TransformData *transformsData, int transformAmount, ChildrenID *childrenData, int childrenAmount)
{
	Transform *transforms = mem_alloc_i(sizeof(Transform) * transformAmount);

	Transform *tf;
	TransformData *tfd;

	//Transform
	//int childrenTotal = 0;
	int haveChildCount = 0;
	//设定世界坐标
	for (int i = 0; i < transformAmount; i++)
	{
		tf = &transforms[i];
		tfd = &transformsData[i];

		tf->position = tfd->position;
		tf->rotation.right = tfd->right;
		tf->rotation.up = tfd->up;
		xyz_cross(&tf->rotation.forward, &tf->rotation.right, &tf->rotation.up);

		tf->entity = NULL;
		tf->parent = NULL;
		if (tfd->childCount == 0) tf->children = NULL;
		else haveChildCount++;
	}

	Children *childrenSpace = mem_alloc_i(sizeof(Children) * haveChildCount + sizeof(Transform *) * childrenAmount);
	Children *thisChildren = childrenSpace;
	int thisChildrenCount;
	int childrenDataIndex = 0;
	//设定父子关系和局部坐标
	for (int i = 0; i < transformAmount; i++)
	{
		tf = &transforms[i];
		tfd = &transformsData[i];
		thisChildrenCount = tfd->childCount;

		if (thisChildrenCount != 0)
		{
			tf->children = thisChildren;
			tf->children->count = thisChildrenCount;
			//设定children.list
			for (int c = 0; c < thisChildrenCount; c++)
			{
				//因为entity为空，所以不能使用_SetPosition等
				//且child的children未确定
				//_SetChild(tf, c, &transforms[childrenData[childrenDataIndex + c]]);

				Transform *child = &transforms[childrenData[childrenDataIndex + c]];
				tf->children->list[c] = child;
				child->parent = tf;

				TransformPointToLocal(&tf->rotation, &tf->position, &child->position, &child->localPosition);
				TransformMatrixToLocal(&tf->rotation, &child->rotation, &child->localRotation);
			}

			thisChildren = (char *)thisChildren + sizeof(Children) + sizeof(Transform *) * thisChildrenCount;
			childrenDataIndex += thisChildrenCount;
		}
	}

	return transforms;
}


Scene *LoadScene(SceneData *sceneData)
{
	//EntityData *entitiesData = sceneData->data;
	EntityData *entitiesData = (EntityData *)&sceneData->data[0];
	TransformData *transformsData = &entitiesData[sceneData->totalAmount];
	ChildrenID *childrenData = &transformsData[sceneData->TransformAmount];
	//Shape,Materials,Others
	DataIdentity *dataInfos = &childrenData[sceneData->childrenAmount];

	Scene *scene = mem_alloc_i((sizeof(Scene) + sizeof(Entity) * sceneData->totalAmount));
	Transform *transforms = LoadTransforms(transformsData, sceneData->TransformAmount, childrenData, sceneData->childrenAmount);

	Entity *et;
	EntityData *etd;
	for (int i = 0; i < sceneData->totalAmount; i++)
	{
		et = &scene->entities[i];
		etd = &entitiesData[i];

		et->transform = &transforms[etd->transformID];
		et->transform->entity = et;
		et->property = etd->property;
		et->ptMaterial = DeserializeData(dataInfos, etd->materialID);
		et->ptShape = DeserializeData(dataInfos, etd->shapeID);

		//更新形状世界坐标
		et->ptShape->fc->OnTransformUpdate(et);
	}

	scene->totalCount = sceneData->totalAmount;
	scene->objectCount = sceneData->objectAmount;
	scene->transforms = transforms;
	fp16 ao = fp16_100f(5);
	color_set(&scene->ambientLightColor, ao, ao, ao);

	scene->camera = GetTestCamera();
	//scene->GetSkyColor = GetSunSkyColor;
	//scene->GetSkyColor = GetSpaceColor;
	scene->GetSkyColor = GetBlackBackground;
	//scene->animation = &UpdataScene
	return scene;
}
