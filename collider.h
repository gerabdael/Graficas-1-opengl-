#pragma once

#include <string>
#include "axisalignedboundingbox.h"

class Collider
{
public:
	enum Type
	{
		BOUNDING_POINT,
		BOUNDING_SPHERE,
		AXIS_ALIGNED_BOUNGING_BOX
	} type;

	AxisAlignedBoundingBox aabb;
	BoundingSphere bs;
	BoundingPoint bp;

	Collider();
	~Collider();

	GLvoid Update(mat4);
	GLboolean Collides(Collider*);
};

