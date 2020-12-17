#pragma once

#include "material.h"
#include "collider.h"

class Mesh
{
public:

	struct Vertex
	{
		vec3 position;
		vec2 texCoord;
		vec3 normal;
		vec3 tangent;
		vec3 binormal;

		GLboolean operator == (Vertex a) {
			return (position.x == a.position.x
				&& position.y == a.position.y
				&& position.z == a.position.z
				&& texCoord.x == a.texCoord.x
				&& texCoord.y == a.texCoord.y
				&& normal.x == a.normal.x
				&& normal.y == a.normal.y
				&& normal.z == a.normal.z);
		}
	};
	
	string materialName;
	GLuint materialIndex;

	GLuint size, offset;

	Mesh();
	~Mesh();

	GLvoid Draw(Program*, Material*, mat4);
};

