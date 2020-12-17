#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

#include "platform.hpp"
#include "tdogl\Program.h"
#include "tdogl\Texture.h"

using namespace tdogl;
using namespace glm;
using namespace std;

class Sphere
{
public:
	Sphere(Program*, GLfloat, GLfloat, GLfloat);
	~Sphere();

	GLvoid Draw(GLuint, mat4);

private:

	struct Vertex {
		vec3 position;
		vec2 texCoord;
	};

	Program *gProgram;
	GLuint gVAO;
};

