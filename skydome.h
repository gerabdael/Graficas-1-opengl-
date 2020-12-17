#pragma once

#include "sphere.h"

class Skydome
{
public:
	Skydome(Program*, GLfloat);
	~Skydome();

	GLvoid Draw(GLuint, mat4, mat4);

private:
	Program* gProgram;
	Sphere* gSphere;
	GLfloat gRatio;
};

