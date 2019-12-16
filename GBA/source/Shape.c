#include "RT_base.h"
#include "Shape.h"

void OnTransformUpdate_Sphere(Entity *et)
{
	return;
}

void OnTransformUpdate_Plane(Entity *et)
{
	return;
}

void OnTransformUpdate_Box(Entity *et)
{
	return;
}

void OnTransformUpdate_Triangle(Entity *et)
{
	return;
}

void OnTransformUpdate_Mesh(Entity *et)
{
	Transform *tf = et->transform;
	Mesh *mesh = (Mesh *)et->ptShape;
	Vector3 temp;
	Vector3 max = { FP32_MIN, FP32_MIN, FP32_MIN };
	Vector3 min = { FP32_MAX, FP32_MAX, FP32_MAX };

	for (int i = 0; i < mesh->verticesCount; i++)
	{
		TransformToWorld(&tf->rotation, &mesh->vertices[i], &temp);
		xyz_add(&temp, &tf->position, &temp);
		mesh->worldVertices[i] = temp;
		if (mesh->ptBS != NULL)
		{
			if (temp.x > max.x) max.x = temp.x;
			else if (temp.x < min.x) min.x = temp.x;
			if (temp.y > max.y) max.y = temp.y;
			else if (temp.y < min.y) min.y = temp.y;
			if (temp.z > max.z) max.z = temp.z;
			else if (temp.z < min.z) min.z = temp.z;
		}
	}
	for (int j = 0; j < mesh->normalsCount; j++)
	{
		TransformToWorld(&tf->rotation, &mesh->normals[j], &temp);
		mesh->worldNormals[j] = temp;

	}

	if (mesh->ptBS != NULL)
	{
		xyz_scale(&temp, xyz_sub(&temp, &max, &min), FP32_H5);
		mesh->ptBS->radius = xyz_len(&temp) + (FP32_1 >> 6);
		xyz_add(&mesh->ptBS->position, &min, &temp);
	}

}