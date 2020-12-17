#pragma once

#include <vector>
#include "mesh.h"
#include "material.h"

using namespace std;

class Group
{
public:
	string name;
	vector<Mesh> meshes;

	Collider collider;
	GLuint VAO;

	Group();
	~Group();

	GLvoid Draw(Program*, vector<Material>&, mat4);
	GLvoid Update(mat4);

	GLboolean Collides(Group*);
};

