#include "mesh.h"

Mesh::Mesh() { }

Mesh::~Mesh() { }

void Mesh::Draw(Program* program, Material* material, mat4 matrix)
{
	program->setUniform("ambient", material->ambient);
	program->setUniform("diffuse", material->diffuse);
	program->setUniform("specular", material->specular);
	program->setUniform("shininess", material->shininess);
	program->setUniform("transparency", material->transparency);

	program->setUniform("hasNormalMap", material->hasNormalMap());
	program->setUniform("hasAmbientMap", material->hasAmbientMap());
	program->setUniform("hasDiffuseMap", material->hasDiffuseMap());
	program->setUniform("hasSpecularMap", material->hasSpecularMap());
	program->setUniform("hasShininessMap", material->hasShininessMap());
	program->setUniform("hasTransparencyMap", material->hasTransparencyMap());

	if (material->hasNormalMap()) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->normalMap->object());
		program->setUniform("normalMap", 0);
	}

	if (material->hasAmbientMap()) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->ambientMap->object());
		program->setUniform("ambientMap", 1);
	}

	if (material->hasDiffuseMap()) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material->diffuseMap->object());
		program->setUniform("diffuseMap", 2);
	}

	if (material->hasSpecularMap()) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, material->specularMap->object());
		program->setUniform("specularMap", 3);
	}

	if (material->hasShininessMap()) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, material->shininessMap->object());
		program->setUniform("shininessMap", 4);
	}

	if (material->hasTransparencyMap()) {
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, material->transparencyMap->object());
		program->setUniform("transparencyMap", 5);
	}

	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
}