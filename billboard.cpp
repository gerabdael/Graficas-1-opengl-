#include "billboard.h"

Billboard::Billboard(Program* program) 
{
	gQuad = new Quad(program);
	gProgram = program;
}

Billboard::~Billboard() 
{
	delete gQuad;
}

GLvoid Billboard::DrawH(GLuint texture, mat4 cameraMatrix, vec3 cameraPosition, vec3 position, GLfloat width, GLfloat height)
{
	vec3 up = vec3(0, 1, 0);
	GLfloat angle = 0.0f;
		
	//angleH = atan2(position.x - cameraPosition.x, position.z - cameraPosition.z) * (180.0 / pi<float>());

	vec3 objToCam = cameraPosition - position;
	objToCam.y = 0;
	objToCam = normalize(objToCam);

	GLfloat angleCosine = dot(vec3(0, 0, -1), objToCam);
	up = cross(vec3(0, 0, -1), objToCam);

	angle = acos(angleCosine) * (180.0 / pi<float>());

	mat4 modelMatrix =
		translate(mat4(), position) *
		rotate(mat4(), radians(angle), up) *
		translate(mat4(), vec3(-width/2.0f, -height / 2.0f, 0)) *
		scale(mat4(), vec3(width, height, 1));

	gProgram->use();
	gProgram->setUniform("camera", cameraMatrix);
	gProgram->setUniform("alpha", 1.0f);
	gProgram->stopUsing();

	gQuad->Draw(texture, modelMatrix);
}

GLvoid Billboard::DrawV(GLuint texture, mat4 cameraMatrix, vec3 cameraPosition, vec3 position, GLfloat width, GLfloat height)
{
	vec3 up = vec3(0, 1, 0);
	GLfloat angle = 0.0f;

	vec3 objToCam = cameraPosition - position;
	objToCam.x = 0;
	objToCam = normalize(objToCam);

	GLfloat angleCosine = dot(vec3(0, 0, -1), objToCam);
	up = cross(vec3(0, 0, -1), objToCam);

	angle = acos(angleCosine) * (180.0 / pi<float>());

	mat4 modelMatrix =
		translate(mat4(), position) *
		rotate(mat4(), radians(angle), up) *
		translate(mat4(), vec3(-width / 2.0f, -height / 2.0f, 0)) *
		scale(mat4(), vec3(width, height, 1));

	gProgram->use();
	gProgram->setUniform("camera", cameraMatrix);
	gProgram->setUniform("alpha", 1.0f);
	gProgram->stopUsing();

	gQuad->Draw(texture, modelMatrix);
}

GLvoid Billboard::DrawHV(GLuint texture, mat4 cameraMatrix, vec3 cameraPosition, vec3 position, GLfloat width, GLfloat height)
{
	vec3 up = vec3(0, 1, 0);
	GLfloat angle = 0.0f;

	vec3 objToCam = cameraPosition - position;
	objToCam = normalize(objToCam);

	GLfloat angleCosine = dot(vec3(0, 0, -1), objToCam);
	up = cross(vec3(0, 0, -1), objToCam);

	angle = acos(angleCosine) * (180.0 / pi<float>());

	mat4 modelMatrix =
		translate(mat4(), position) *
		rotate(mat4(), radians(angle), up) *
		translate(mat4(), vec3(-width / 2.0f, -height / 2.0f, 0)) *
		scale(mat4(), vec3(width, height, 1));

	gProgram->use();
	gProgram->setUniform("camera", cameraMatrix);
	gProgram->setUniform("alpha", 1.0f);
	gProgram->stopUsing();

	gQuad->Draw(texture, modelMatrix);
}