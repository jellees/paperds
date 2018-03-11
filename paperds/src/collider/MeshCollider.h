#pragma once

#include "Collider.h"

class SphereCollider;

class MeshCollider : public Collider
{
private:
	kcl_header* _collision;

	bool Sphere(SphereCollider *sphere);

public:
	MeshCollider(kcl_header* kcl);
	~MeshCollider();

	bool Collide(Collider* collider);

	Type GetType()
	{
		return MESH;
	}
};