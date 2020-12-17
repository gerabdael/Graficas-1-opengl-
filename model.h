#pragma once

#include <fstream>
#include "group.h"

class Model
{
private:
	Model();

	static vector <Material> LoadMtl(string, string);
	static vector <Group> LoadObj(Program*, string, string, vec3&, vec3&, string&);
	static GLvoid ComputeTangentSpace(vector<Mesh::Vertex>&, vector <GLuint>&);
	static GLvoid CreateBuffers(Program*, Group&, vector<Mesh::Vertex>&, vector<GLuint>&);
	static Collider CalculateCollider(vec3&, vec3&);
	
public:
	vector<Group> groups;
	vector<Material> materials;

	Collider collider;

	~Model();

	static Model *ObjToModel(Program* , string, string);
	
	GLvoid Draw(Program*, mat4);
	GLvoid Update(mat4);

	GLboolean Collides(Model*);
};