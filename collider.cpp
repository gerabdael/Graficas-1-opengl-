#include "collider.h"

Collider::Collider() {
	type = BOUNDING_SPHERE;
}

Collider::~Collider() { }

void Collider::Update(mat4 matrix)
{
	aabb.Update(matrix);
	bs.Update(matrix);
	bp.Update(matrix);
}

GLboolean Collider::Collides(Collider *collider)
{
	switch (type)
	{
		case BOUNDING_POINT:
			switch (collider->type)
			{
				case BOUNDING_POINT: return collider->bp.Collides(&bp);
				case BOUNDING_SPHERE: return collider->bs.Collides(&bp);
				case AXIS_ALIGNED_BOUNGING_BOX: return collider->aabb.Collides(&bp);
			}
			break;
		case BOUNDING_SPHERE:
			switch (collider->type)
			{
			case BOUNDING_POINT: return bs.Collides(&collider->bp);
			case BOUNDING_SPHERE: return collider->bs.Collides(&bs);
			case AXIS_ALIGNED_BOUNGING_BOX: return collider->aabb.Collides(&bs);
			}
			break;
		case AXIS_ALIGNED_BOUNGING_BOX:
			switch (collider->type)
			{
			case BOUNDING_POINT: return aabb.Collides(&collider->bp);
			case BOUNDING_SPHERE: return aabb.Collides(&collider->bs);
			case AXIS_ALIGNED_BOUNGING_BOX: return collider->aabb.Collides(&aabb);
			}
			break;
	}

	return false;
}