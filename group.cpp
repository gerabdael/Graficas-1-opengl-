#include "group.h"

Group::Group() { }

Group::~Group() { }

void Group::Draw(Program* program, vector<Material> &materials, mat4 matrix)
{
	program->setUniform("model", matrix);
	glBindVertexArray(VAO);
	for (auto& mesh : meshes) mesh.Draw(program, &materials.at(mesh.materialIndex), matrix);
	glBindVertexArray(0);
}

void Group::Update(mat4 matrix)
{
	collider.Update(matrix);
}

GLboolean Group::Collides(Group *group)
{
	return collider.Collides(&(group->collider));
}