#include "boundingpoint.h"

BoundingPoint::~BoundingPoint() {};

BoundingPoint::BoundingPoint()
{
	initialPosition = transformedPosition = vec3(0, 0, 0);
};

BoundingPoint::BoundingPoint(vec3 position)
{
	initialPosition = transformedPosition = position;
};

GLvoid BoundingPoint::Update(mat4 matrix)
{
	vec3 translate = vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
	transformedPosition = initialPosition + translate;
}

GLboolean BoundingPoint::Collides(BoundingPoint *point)
{
	float distance = glm::distance(transformedPosition, point->transformedPosition);
	return distance <= 0.01;
}