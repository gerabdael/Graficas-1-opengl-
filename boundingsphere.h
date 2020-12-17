#pragma once

#include "boundingpoint.h"

class BoundingSphere
{
public:
	vec3 initialPosition, transformedPosition;
	GLfloat initialRadius, transformedRadius;

	~BoundingSphere();
	BoundingSphere();
	BoundingSphere(vec3, GLfloat);
	
	GLvoid Update(mat4);

	GLboolean Collides(BoundingSphere*);
	GLboolean Collides(BoundingPoint*);
};

