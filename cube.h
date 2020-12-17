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

class Cube
{
public:
	Cube(Program*);
	~Cube();

	void Draw(GLuint, mat4);

private:
	Program *gProgram;
	GLuint gVAO;
};

