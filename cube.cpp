#include "Cube.h"

Cube::Cube(Program *_gProgram)
{
	gProgram = _gProgram;
	GLuint gVBO;

	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);

	GLfloat vertexData[] = {
		// bottom
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
		 1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,

		// top
		-1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
		 1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

		 // front
		 -1.0f,-1.0f, 1.0f,   1.0f, 0.0f,
		  1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
		 -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
		  1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
		  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
		 -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

		 // back
		 -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
		 -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
		  1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
		  1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
		 -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
		  1.0f, 1.0f,-1.0f,   1.0f, 1.0f,

		  // left
		  -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
		  -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
		  -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
		  -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
		  -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
		  -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,

		  // right
		  1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
		  1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
		  1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
		  1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
		  1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
		  1.0f, 1.0f, 1.0f,   0.0f, 1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(gProgram->attrib("vertPosition"));
	glVertexAttribPointer(gProgram->attrib("vertPosition"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(gProgram->attrib("vertTexCoord"));
	glVertexAttribPointer(gProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
}

Cube::~Cube()
{
}

GLvoid Cube::Draw(GLuint texture, mat4 modelMatrix)
{
	gProgram->use();

	gProgram->setUniform("model", modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	gProgram->setUniform("map", 0);

	glBindVertexArray(gVAO);

	glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	gProgram->stopUsing();
}