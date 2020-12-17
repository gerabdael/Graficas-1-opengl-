#include "skydome.h"

Skydome::~Skydome() { }

Skydome::Skydome(Program* _gProgram, GLfloat ratio)
{
	gProgram = _gProgram;
	gRatio = ratio;
	gSphere = new Sphere(_gProgram, ratio, 0.5f, 1.0f);
}

GLvoid Skydome::Draw(GLuint texture, mat4 cameraMatrix, mat4 modelMatrix)
{
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);


	gProgram->use();
	gProgram->setUniform("camera", cameraMatrix);
	gProgram->setUniform("alpha", 1.0f);
	gProgram->stopUsing();

	gSphere->Draw(texture, modelMatrix);

	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}