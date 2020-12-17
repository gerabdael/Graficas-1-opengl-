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

class HeightMesh
{
public:
	struct HeightMeshVertex {
		vec3 position;
		vec2 tileTexCoord;
		vec2 fullTexCoord;
		vec3 normal;
	};

	HeightMesh(Program*, string, vector<string>, GLuint, GLuint, GLuint, GLuint);
	HeightMesh(Program*, GLfloat, vector<string>, GLuint, GLuint, GLuint);
	
	~HeightMesh();

	GLvoid Draw(mat4, GLfloat = 0.0f);

	GLfloat GetAverageHeight(GLfloat, GLfloat);
	GLfloat GetBIHeight(GLfloat, GLfloat);

private:

	GLvoid Create(Bitmap, GLuint, GLuint, GLuint, GLuint);

	GLvoid SmoothHeights(GLfloat);
	GLvoid ComputePositions(HeightMeshVertex*);
	GLvoid ComputeTexCoords(HeightMeshVertex*, GLuint);
	GLvoid ComputeNormals(HeightMeshVertex*);
	GLvoid SmoothNormals(HeightMeshVertex*);

	GLvoid ComputeVAO(HeightMeshVertex*);

	GLfloat GetMapHeight(GLfloat, GLfloat);
	GLfloat GetTerrainHeight(GLfloat, GLfloat);

	Program* gProgram;
	GLuint gVAO;

	GLfloat *height;
	vector<Texture*> textures;

	GLuint vertexCount, indexCount;
	GLuint mapWidth, mapHeight;
	GLuint terrainWidth, terrainHeight, terrainDepth;
	GLfloat deltaX, deltaY, deltaZ;
};

