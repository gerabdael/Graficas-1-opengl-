#pragma once

#include "quad.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

class GUI
{
public:
	GUI(Program*, string);
	~GUI();

	GLvoid Draw(mat4, GLuint, mat4, GLfloat = 1.0f);
	GLvoid Draw(mat4, GLuint, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat = 0.0f, GLfloat = 1.0f);

	GLvoid DrawFont(mat4, string, GLfloat, GLfloat, GLfloat, vec3, GLfloat = 1.0f);

private:
	Quad* gQuad;
	Program *gProgram;

	struct Character {
		GLuint TextureID;
		ivec2 Size;
		ivec2 Bearing;
		GLuint Advance;
	};

	map<GLchar, Character> Characters;
};

