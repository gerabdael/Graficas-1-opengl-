#include "boundingsphere.h"

BoundingSphere::~BoundingSphere() {};

BoundingSphere::BoundingSphere()
{
	initialPosition = transformedPosition = vec3(0, 0, 0);
	initialRadius = transformedRadius = 0.0f;
};

BoundingSphere::BoundingSphere(vec3 position, GLfloat radius)
{
	initialPosition = transformedPosition = position;
	initialRadius = transformedRadius = radius;
};

GLvoid BoundingSphere::Update(mat4 matrix)
{
	vec3 scale = vec3(matrix[0][0], matrix[1][1], matrix[2][2]);
	vec3 translate = vec3(matrix[3][0], matrix[3][1], matrix[3][2]);

	transformedPosition = initialPosition + translate;
	float size = max(max(scale.x, scale.y), scale.z);
	transformedRadius = initialRadius * size;
}

GLboolean BoundingSphere::Collides(BoundingSphere *sphere)
{
	float distance = glm::distance(transformedPosition, sphere->transformedPosition);
	return distance <= transformedRadius + sphere->transformedRadius;
}

GLboolean BoundingSphere::Collides(BoundingPoint *point)
{
	float distance = glm::distance(transformedPosition, point->transformedPosition);
	return distance <= transformedRadius;
}