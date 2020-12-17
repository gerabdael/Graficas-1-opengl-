#include "heightMesh.h"

HeightMesh::~HeightMesh() {}

HeightMesh::HeightMesh(Program* program, string _heightmap, vector<string> _textures, GLuint sizeX, GLuint sizeY, GLuint sizeZ, GLuint tiles)
{
	gProgram = program;

	Bitmap heightMap = Bitmap::bitmapFromFile(_heightmap);

	for (GLuint i = 0; i < _textures.size(); i++) textures.push_back(new Texture(Bitmap::bitmapFromFile(_textures.at(i))));

	Create(heightMap, sizeX, sizeY, sizeZ, tiles);
}

HeightMesh::HeightMesh(Program* program, GLfloat height, vector<string> _textures, GLuint sizeX, GLuint sizeZ, GLuint tiles)
{
	gProgram = program;

	GLubyte *map = new GLubyte[sizeX * sizeZ * 3];
	memset(map, 255, (sizeX * sizeZ * 3) * sizeof(GLubyte));

	Bitmap heightMap = Bitmap(sizeX, sizeZ, Bitmap::Format_RGB, map);

	for (GLuint i = 0; i < _textures.size(); i++) textures.push_back(new Texture(Bitmap::bitmapFromFile(_textures.at(i))));

	Create(heightMap, sizeX, height, sizeZ, tiles);
}

GLvoid HeightMesh::Create(Bitmap heightMap, GLuint sizeX, GLuint sizeY, GLuint sizeZ, GLuint tiles)
{
	mapWidth = heightMap.width();
	mapHeight = heightMap.height();

	height = new GLfloat[mapWidth * mapHeight];
	
	for (int i = 0; i < (mapWidth * mapHeight); i++) height[i] = (GLfloat)heightMap.pixelBuffer()[i * 3];

	terrainWidth = sizeX;
	terrainHeight = sizeY;
	terrainDepth = sizeZ;

	SmoothHeights(0.8f);

	HeightMeshVertex* heightMapVertices = new HeightMeshVertex[mapHeight * mapWidth];

	ComputePositions(heightMapVertices);
	ComputeTexCoords(heightMapVertices, tiles);
	ComputeNormals(heightMapVertices);

	ComputeVAO(heightMapVertices);
}

GLvoid HeightMesh::SmoothHeights(GLfloat k)
{
	for (GLuint x = 1; x < mapWidth; x++)
	{
		for (GLuint z = 0; z < mapHeight; z++)
		{
			GLuint index1 = (x * mapWidth) + z;
			GLuint index2 = ((x - 1) * mapWidth) + z;

			height[index1] = height[index2] * (1 - k) + height[index1] * k;
		}
	}
		
	for (GLuint x = (mapWidth - 2); x < -1; x--)
	{
		for (GLuint z = 0; z < mapHeight; z++)
		{
			GLuint index1 = (x * mapWidth) + z;
			GLuint index2 = ((x + 1) * mapWidth) + z;

			height[index1] = height[index2] * (1 - k) + height[index1] * k;
		}
	}

	for (GLuint x = 0; x < mapWidth; x++)
	{
		for (GLuint z = 1; z < mapHeight; z++)
		{
			GLuint index1 = (x * mapWidth) + z;
			GLuint index2 = (x * mapWidth) + (z - 1);

			height[index1] = height[index2] * (1 - k) + height[index1] * k;
		}
	}

	for (GLuint x = 0; x < mapWidth; x++)
	{
		for (GLuint z = (mapHeight - 2); z < -1; z--)
		{
			GLuint index1 = (x * mapWidth) + z;
			GLuint index2 = (x * mapWidth) + (z + 1);

			height[index1] = height[index2] * (1 - k) + height[index1] * k;
		}
	}
}

GLvoid HeightMesh::ComputePositions(HeightMeshVertex* vertices)
{
	deltaX = (GLfloat)terrainWidth / (GLfloat)mapWidth;
	deltaY = (GLfloat)terrainHeight / (GLfloat)255;
	deltaZ = (GLfloat)terrainDepth / (GLfloat)mapHeight;

	for (GLuint j = 0; j < mapHeight; j++)
	{
		for (GLuint i = 0; i < mapWidth; i++)
		{
			GLuint index = (j * mapWidth) + i;

			vertices[index].position = vec3(i * deltaX, height[index] * deltaY, j * deltaZ);
		}
	}
}

GLvoid HeightMesh::ComputeTexCoords(HeightMeshVertex* vertices, GLuint tiles)
{
	GLfloat incrementSizeWidth = 1.0f / (GLfloat)mapWidth;
	GLfloat incrementSizeHeight = 1.0f / (GLfloat)mapHeight;

	GLfloat tu2Left = 0.0f;
	GLfloat tu2Right = incrementSizeWidth;
	GLfloat tv2Bottom = 1.0f;
	GLfloat tv2Top = 1.0f - incrementSizeHeight;

	for (GLuint j = 0; j < (mapHeight - 1); j++)
	{
		for (GLuint i = 0; i < (mapWidth - 1); i++)
		{
			GLuint index1 = (mapWidth * j) + i;				// Bottom left.
			GLuint index2 = (mapWidth * j) + (i + 1);		// Bottom right.
			GLuint index3 = (mapWidth * (j + 1)) + i;		// Upper left.
			GLuint index4 = (mapWidth * (j + 1)) + (i + 1);	// Upper right.

			//vertices[index1].tileTexCoord = vec2(0.0f, 0.0f);
			//vertices[index2].tileTexCoord = vec2(0.0f, 1.0f);
			//vertices[index3].tileTexCoord = vec2(1.0f, 0.0f);
			//vertices[index4].tileTexCoord = vec2(1.0f, 1.0f);

			vertices[index1].tileTexCoord = vec2(tu2Left * tiles, tv2Bottom * tiles);
			vertices[index2].tileTexCoord = vec2(tu2Right * tiles, tv2Bottom * tiles);
			vertices[index3].tileTexCoord = vec2(tu2Left * tiles, tv2Top * tiles);
			vertices[index4].tileTexCoord = vec2(tu2Right * tiles, tv2Top * tiles);

			vertices[index1].fullTexCoord = vec2(tu2Left, tv2Bottom);
			vertices[index2].fullTexCoord = vec2(tu2Right, tv2Bottom);
			vertices[index3].fullTexCoord = vec2(tu2Left, tv2Top);
			vertices[index4].fullTexCoord = vec2(tu2Right, tv2Top);

			tu2Left += incrementSizeWidth;
			tu2Right += incrementSizeWidth;
		}

		tu2Left = 0.0f;
		tu2Right = incrementSizeWidth;

		tv2Top += incrementSizeHeight;
		tv2Bottom += incrementSizeHeight;
	}
}

GLvoid HeightMesh::ComputeNormals(HeightMeshVertex* vertices)
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;

	for (j = 0; j < (mapHeight - 1); j++)
	{
		for (i = 0; i < (mapWidth - 1); i++)
		{
			GLuint index1 = (mapWidth * j) + i;				// Bottom left.
			GLuint index2 = (mapWidth * j) + (i + 1);		// Bottom right.
			GLuint index3 = (mapWidth * (j + 1)) + i;		// Upper left.

			vec3 vertex1 = vertices[index1].position;
			vec3 vertex2 = vertices[index2].position;
			vec3 vertex3 = vertices[index3].position;

			vec3 vector1 = vertex1 - vertex3;
			vec3 vector2 = vertex3 - vertex2;

			GLuint index = (j * mapWidth) + i;
			vertices[index].normal = cross(vector1, vector2);
		}
	}

	SmoothNormals(vertices);
}

GLvoid HeightMesh::SmoothNormals(HeightMeshVertex* vertices)
{
	vec3 *normals = new vec3[mapWidth * mapHeight];

	for (GLuint j = 0; j < (mapHeight - 1); j++)
	{
		for (GLuint i = 0; i < (mapWidth - 1); i++)
		{
			GLuint index1 = (mapWidth * j) + i;				// Bottom left.
			GLuint index2 = (mapWidth * j) + (i + 1);		// Bottom right.
			GLuint index3 = (mapWidth * (j + 1)) + i;		// Upper left.
			GLuint index4 = (mapWidth * (j + 1)) + (i + 1);	// Upper right.

			vec3 sum(0.0f);
			GLuint count = 0;

			sum += vertices[index1].normal;
			sum += vertices[index2].normal;
			sum += vertices[index3].normal;
			sum += vertices[index4].normal;

			GLuint index = (j * mapWidth) + i;
			normals[index] = sum / 4.0f;
		}
	}

	for (GLuint i = 0; i < ((mapHeight-1) * (mapHeight-1)); i++) vertices[i].normal = normalize(normals[i]);
}

GLvoid HeightMesh::ComputeVAO(HeightMeshVertex* vertices)
{
	vertexCount = mapWidth * mapHeight;
	indexCount = mapWidth * mapHeight * 6;

	GLuint* indices = new GLuint[indexCount];

	GLuint index = 0;

	for (GLuint j = 0; j < (mapHeight - 1); j++)
	{
		for (GLuint i = 0; i < (mapWidth - 1); i++)
		{
			GLuint index1 = (mapWidth * j) + i;				// Bottom left.
			GLuint index2 = (mapWidth * j) + (i + 1);		// Bottom right.
			GLuint index3 = (mapWidth * (j + 1)) + i;		// Upper left.
			GLuint index4 = (mapWidth * (j + 1)) + (i + 1);	// Upper right.

			indices[index++] = index3;
			indices[index++] = index2;
			indices[index++] = index1;

			indices[index++] = index2;
			indices[index++] = index3;
			indices[index++] = index4;
		}
	}

	GLuint gVBO, gIBO;

	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(HeightMeshVertex) * vertexCount, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(gProgram->attrib("vertPosition"));
	glVertexAttribPointer(gProgram->attrib("vertPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(HeightMeshVertex), 0);

	glEnableVertexAttribArray(gProgram->attrib("vertTileTexCoord"));
	glVertexAttribPointer(gProgram->attrib("vertTileTexCoord"), 2, GL_FLOAT, GL_FALSE, sizeof(HeightMeshVertex), (const GLvoid*)(sizeof(vec3)));

	glEnableVertexAttribArray(gProgram->attrib("vertFullTexCoord"));
	glVertexAttribPointer(gProgram->attrib("vertFullTexCoord"), 2, GL_FLOAT, GL_FALSE, sizeof(HeightMeshVertex), (const GLvoid*)(sizeof(vec3) + sizeof(vec2)));

	glEnableVertexAttribArray(gProgram->attrib("vertNormal"));
	glVertexAttribPointer(gProgram->attrib("vertNormal"), 3, GL_FLOAT, GL_TRUE, sizeof(HeightMeshVertex), (const GLvoid*)(sizeof(vec3) + sizeof(vec2) + sizeof(vec2)));

	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexCount, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLvoid HeightMesh::Draw(mat4 cameraMatrix, GLfloat textureMovement)
{
	gProgram->use();

	gProgram->setUniform("camera", cameraMatrix);

	if (textureMovement > 0.0f) gProgram->setUniform("movement", textureMovement);

	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures.at(i)->object());
		string name = "map" + to_string(i);
		gProgram->setUniform(name.c_str(), i);
	}

	glBindVertexArray(gVAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	gProgram->stopUsing();
}

GLfloat HeightMesh::GetMapHeight(GLfloat x, GLfloat z)
{
	if (x < 0 || z < 0 || x >= mapWidth || z >= mapHeight) return 0.0f;
	GLuint index = (mapWidth * z) + x;
	return height[index];
}

GLfloat HeightMesh::GetTerrainHeight(GLfloat posX, GLfloat posZ)
{
	GLuint x = posX / deltaX;
	GLuint z = posZ / deltaZ;
	return GetMapHeight(x, z) * deltaY;
}

GLfloat HeightMesh::GetAverageHeight(GLfloat posX, GLfloat posZ)
{
	GLuint x = posX / deltaX;
	GLuint z = posZ / deltaZ;

	GLfloat BL = GetMapHeight(x + 0, z + 0) * deltaY;
	GLfloat BR = GetMapHeight(x + 0, z + 1) * deltaY;
	GLfloat TL = GetMapHeight(x + 1, z + 0) * deltaY;
	GLfloat TR = GetMapHeight(x + 1, z + 1) * deltaY;

	return (TL + TR + BL + BR) / 4.0f;
}

//Bilinear interpolation no funciona correctamente
GLfloat HeightMesh::GetBIHeight(GLfloat posX, GLfloat posZ)
{
	GLuint x = posX / deltaX;
	GLuint z = posZ / deltaZ;

	GLfloat BL = GetMapHeight(x + 0, z + 0) * deltaY;
	GLfloat BR = GetMapHeight(x + 0, z + 1) * deltaY;
	GLfloat TL = GetMapHeight(x + 1, z + 0) * deltaY;
	GLfloat TR = GetMapHeight(x + 1, z + 1) * deltaY;

	GLfloat PX = posX - (GLint)posX;
	GLfloat PZ = posZ - (GLint)posZ;

	GLfloat AB = TL * (1.0 - PX) + TR * PX;
	GLfloat CD = BL * (1.0 - PX) + BR * PX;
	GLfloat ABCD = AB * (1.0 - PZ) + CD * PZ;

	return ABCD;
}
