#include "axisalignedboundingbox.h"

AxisAlignedBoundingBox::~AxisAlignedBoundingBox() {};

AxisAlignedBoundingBox::AxisAlignedBoundingBox()
{
	initialMinimumPoint = transformedMinimumPoint = vec3(0, 0, 0);
	initialMaximumPoint = transformedMaximumPoint = vec3(0, 0, 0);
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(vec3 MinimumPoint, vec3 MaximumPoint)
{
	initialMinimumPoint = transformedMinimumPoint = MinimumPoint;
	initialMaximumPoint = transformedMaximumPoint = MaximumPoint;
}

GLvoid AxisAlignedBoundingBox::Update(mat4 matrix)
{
	vec3 scale = vec3(matrix[0][0], matrix[1][1], matrix[2][2]);
	vec3 translate = vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
	
	transformedMinimumPoint = initialMinimumPoint * scale + translate;
	transformedMaximumPoint = initialMaximumPoint * scale + translate;
}

GLboolean AxisAlignedBoundingBox::Collides(AxisAlignedBoundingBox *box)
{
	return glm::all(glm::greaterThanEqual(transformedMaximumPoint, box->transformedMinimumPoint))
		&& glm::all(glm::lessThanEqual(transformedMinimumPoint, box->transformedMaximumPoint));
}

GLboolean AxisAlignedBoundingBox::Collides(BoundingSphere *sphere)
{
	vec3 nearestPoint;
	nearestPoint.x = max(transformedMinimumPoint.x, min(sphere->transformedPosition.x, transformedMaximumPoint.x));
	nearestPoint.y = max(transformedMinimumPoint.y, min(sphere->transformedPosition.y, transformedMaximumPoint.y));
	nearestPoint.z = max(transformedMinimumPoint.z, min(sphere->transformedPosition.z, transformedMaximumPoint.z));

	float distance = glm::distance(nearestPoint, sphere->transformedPosition);
	return distance <= sphere->transformedRadius;
}

GLboolean AxisAlignedBoundingBox::Collides(BoundingPoint *point)
{
	return glm::all(glm::greaterThanEqual(point->transformedPosition, transformedMinimumPoint))
		&& glm::all(glm::lessThanEqual(point->transformedPosition, transformedMaximumPoint));
}

