#include "material.h"

Material::Material()
{
	name = "";

	ambient = vec3(0, 0, 0);
	diffuse = vec3(0, 0, 0);
	specular = vec3(0, 0, 0);

	shininess = 1.0f;
	transparency = 1.0f;

	normalMap = NULL;
	ambientMap = NULL;
	diffuseMap = NULL;
	specularMap = NULL;
	shininessMap = NULL;
	transparencyMap = NULL;
	
}

Material::~Material() 
{
	normalMap = NULL;
	ambientMap = NULL;
	diffuseMap = NULL;
	specularMap = NULL;
	shininessMap = NULL;
	transparencyMap = NULL;
}

GLboolean Material::hasNormalMap() {
	return normalMap != NULL;
}

GLboolean Material::hasAmbientMap() {
	return ambientMap != NULL;
}

GLboolean Material::hasDiffuseMap() {
	return diffuseMap != NULL;
}

GLboolean Material::hasSpecularMap() {
	return specularMap != NULL;
}

GLboolean Material::hasShininessMap() {
	return shininessMap != NULL;
}

GLboolean Material::hasTransparencyMap() {
	return transparencyMap != NULL;
}