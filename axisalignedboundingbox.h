#pragma once

#include "BoundingSphere.h"

class AxisAlignedBoundingBox
{
public:
	vec3 initialMinimumPoint, initialMaximumPoint;
	vec3 transformedMinimumPoint, transformedMaximumPoint;

	~AxisAlignedBoundingBox();
	AxisAlignedBoundingBox();
	AxisAlignedBoundingBox(vec3, vec3);
		
	GLvoid Update(mat4);

	GLboolean Collides(AxisAlignedBoundingBox*);
	GLboolean Collides(BoundingSphere*);
	GLboolean Collides(BoundingPoint*);
};

