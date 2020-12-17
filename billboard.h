#pragma once

#include "quad.h"

class Billboard
{
public:
	Billboard(Program*);
	~Billboard();

	GLvoid DrawH(GLuint, mat4, vec3, vec3, GLfloat, GLfloat);
	GLvoid DrawV(GLuint, mat4, vec3, vec3, GLfloat, GLfloat);
	GLvoid DrawHV(GLuint, mat4, vec3, vec3, GLfloat, GLfloat);

private:
	Quad* gQuad;
	Program* gProgram;
};