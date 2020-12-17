#pragma once

#include "platform.hpp"

#include <GL/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>

#include "tdogl/Program.h"
#include "tdogl/Texture.h"

using namespace std;
using namespace glm;
using namespace tdogl;

class Quad
{
public:
	Quad(Program*);
	~Quad();

	GLvoid Draw(GLuint, mat4);
private:
	Program* gProgram;
	GLuint gVAO;
};

