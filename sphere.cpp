#include "Sphere.h"

Sphere::~Sphere() { }

Sphere::Sphere(Program* _gProgram, GLfloat ratio, GLfloat initial, GLfloat final)
{
	gProgram = _gProgram;

	int slices = 36;
	int stacks = 36;

	int m_vertexCount = slices * stacks;
	int m_indexCount = (slices - 1) * (stacks - 1) * 6;

	Vertex* vertices = new Vertex[m_vertexCount];
	if (!vertices) return;

	GLuint* indices = new GLuint[m_indexCount];
	if (!indices) return;

	for (int i = 0; i < slices; i++)
	{
		for (int j = 0; j < stacks; j++)
		{
			int indice = (i * stacks + j);

			vertices[indice].position.x = ratio * cos(((double)j / (stacks - 1.0)) * (pi<double>() * (final - initial)) + pi<double>() * initial - pi<double>() / 2.0) * cos(2.0 * pi<double>() * (double)i / (slices - 1.0));
			vertices[indice].position.y = ratio * sin(((double)j / (stacks - 1.0)) * (pi<double>() * (final - initial)) + pi<double>() * initial - pi<double>() / 2.0);
			vertices[indice].position.z = ratio * cos(((double)j / (stacks - 1.0)) * (pi<double>() * (final - initial)) + pi<double>() * initial - pi<double>() / 2.0) * sin(2.0 * pi<double>() * (double)i / (slices - 1.0));

			vertices[indice].texCoord.x = (1 - (double)i / (stacks - 1.0));
			vertices[indice].texCoord.y = (1 - (double)j / (slices - 1.0));
		}
	}

	int index = 0, index1 = 0, index2 = 0, index3 = 0, index4 = 0;

	for (int i = 0; i < slices - 1; i++)
	{
		for (int j = 0; j < stacks - 1; j++)
		{
			index1 = (stacks * i) + j; 
			index2 = (stacks * i) + (j + 1);
			index3 = (stacks * (i + 1)) + j;
			index4 = (stacks * (i + 1)) + (j + 1);

			indices[index++] = index1;
			indices[index++] = index2;
			indices[index++] = index4;

			indices[index++] = index1;
			indices[index++] = index4;
			indices[index++] = index3;
		}
	}

	GLuint gVBO, gIBO;

	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertexCount, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(gProgram->attrib("vertPosition"));
	glVertexAttribPointer(gProgram->attrib("vertPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(gProgram->attrib("vertTexCoord"));
	glVertexAttribPointer(gProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (const GLvoid*)(sizeof(vec3)));

	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indexCount, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLvoid Sphere::Draw(GLuint texture, mat4 modelMatrix)
{
	gProgram->use();

	gProgram->setUniform("model", modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	gProgram->setUniform("map", 0);

	glBindVertexArray(gVAO);

	glDrawElements(GL_TRIANGLES, (36 - 1)*(36 - 1) * 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	gProgram->stopUsing();
}
