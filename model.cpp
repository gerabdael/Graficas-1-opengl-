#include "model.h"

Model::Model() { }

Model::~Model() { }

GLvoid Model::Draw(Program* program, mat4 matrix)
{
	for (auto &group : groups) group.Draw(program, materials, matrix);
}

GLvoid Model::Update(mat4 matrix)
{
	collider.Update(matrix);
	for (auto &group : groups) group.Update(matrix);
}

GLboolean Model::Collides(Model *model)
{
	if (collider.Collides(&(model->collider)))
		for (auto &group : groups)
			for (auto &group2 : model->groups)
				if (group.Collides(&group2)) return true;

	return false;
}

Model *Model::ObjToModel(Program* program, string relativePath, string fileName) {

	vec3 minimum = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 maximum = vec3(FLT_MIN, FLT_MIN, FLT_MIN);

	string mtlName = "";

	vector<Group> _groups = LoadObj(program, relativePath, fileName, minimum, maximum, mtlName);
	vector<Material> _materials = LoadMtl(relativePath, mtlName);
	
	GLuint index = 0;

	for (auto& group : _groups) {
		for (auto& mesh : group.meshes) {
			for (auto& material : _materials) {
				if (mesh.materialName == material.name)
				{
					mesh.materialIndex = index;
					break;
				}
				index++;
			}
			index = 0;
		}
	}

	Model *_model = new Model();
	_model->groups = _groups;
	_model->materials = _materials;
	_model->collider = CalculateCollider(minimum, maximum);

	return _model;
}

vector <Group> Model::LoadObj(Program* program, string relativePath, string fileName, vec3& minimum, vec3& maximum, string& mtlName)
{
	vector<Group> groupList;

	vector<Mesh::Vertex> vertexBuffer;
	vector<GLuint> indexBuffer;

	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texCoords;

	vec3 modelMin = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 modelMax = vec3(FLT_MIN, FLT_MIN, FLT_MIN);

	vec3 groupMin = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 groupMax = vec3(FLT_MIN, FLT_MIN, FLT_MIN);

	vec3 meshMin = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 meshMax = vec3(FLT_MIN, FLT_MIN, FLT_MIN);

	ifstream file;
	file.open(relativePath + "/" + fileName);
	if (file.is_open())
	{
		while (file.good())
		{
			string token;
			file >> token;

			if (token == "mtllib")
			{
				file >> mtlName;
			}
			else if (token == "g")
			{
				if (!groupList.empty())
				{
					groupList.back().collider = CalculateCollider(groupMin, groupMax);
					groupList.back().meshes.back().size = indexBuffer.size() - groupList.back().meshes.back().offset;

					ComputeTangentSpace(vertexBuffer, indexBuffer);
					CreateBuffers(program, groupList.back(), vertexBuffer, indexBuffer);
				}

				groupList.emplace_back();
				file >> groupList.back().name;
			}
			else if (token == "usemtl")
			{
				if (!groupList.back().meshes.empty())
				{
					groupList.back().meshes.back().size = indexBuffer.size() - groupList.back().meshes.back().offset;
				}
	
				groupList.back().meshes.emplace_back();
				groupList.back().meshes.back().offset = indexBuffer.size();

				file >> groupList.back().meshes.back().materialName;
			}
			else if (token == "v")
			{
				vec3 position;
				file >> position.x >> position.y >> position.z;

				if (position.x > modelMax.x) modelMax.x = position.x;
				if (position.y > modelMax.y) modelMax.y = position.y;
				if (position.z > modelMax.z) modelMax.z = position.z;
				if (position.x < modelMin.x) modelMin.x = position.x;
				if (position.y < modelMin.y) modelMin.y = position.y;
				if (position.z < modelMin.z) modelMin.z = position.z;

				if (position.x > groupMax.x) groupMax.x = position.x;
				if (position.y > groupMax.y) groupMax.y = position.y;
				if (position.z > groupMax.z) groupMax.z = position.z;
				if (position.x < groupMin.x) groupMin.x = position.x;
				if (position.y < groupMin.y) groupMin.y = position.y;
				if (position.z < groupMin.z) groupMin.z = position.z;

				if (position.x > meshMax.x) meshMax.x = position.x;
				if (position.y > meshMax.y) meshMax.y = position.y;
				if (position.z > meshMax.z) meshMax.z = position.z;
				if (position.x < meshMin.x) meshMin.x = position.x;
				if (position.y < meshMin.y) meshMin.y = position.y;
				if (position.z < meshMin.z) meshMin.z = position.z;

				positions.push_back(position);
			}
			else if (token == "vt")
			{
				vec2 texCoord;
				file >> texCoord.x >> texCoord.y;
				texCoords.push_back(texCoord);
			}
			else if (token == "vn")
			{
				vec3 normal;
				file >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (token[0] == 'f')
			{
				for (unsigned int i = 0; i < 3; i++)
				{
					unsigned int index, positionIndex, texCoordIndex, normalIndex;

					file >> positionIndex;
					file.ignore(INT_MAX, '/');
					file >> texCoordIndex;
					file.ignore(INT_MAX, '/');
					file >> normalIndex;

					Mesh::Vertex vertex;
					vertex.position = positions[positionIndex - 1];
					vertex.texCoord = texCoords[texCoordIndex - 1];
					vertex.normal = normals[normalIndex - 1];

					index = find(vertexBuffer.begin(), vertexBuffer.end(), vertex) - vertexBuffer.begin();

					if (index < vertexBuffer.size())
					{
						indexBuffer.push_back(index);
					}
					else
					{
						vertexBuffer.push_back(vertex);
						indexBuffer.push_back(vertexBuffer.size() - 1);
					}
				}
			}
			file.ignore(INT_MAX, '\n');
		}
		file.close();
	}

	groupList.back().collider = CalculateCollider(groupMin, groupMax);
	groupList.back().meshes.back().size = indexBuffer.size();

	ComputeTangentSpace(vertexBuffer, indexBuffer);
	CreateBuffers(program, groupList.back(), vertexBuffer, indexBuffer);

	minimum = modelMin;
	maximum = modelMax;

	return groupList;
}

GLvoid Model::ComputeTangentSpace(vector <Mesh::Vertex> &vertices, vector <GLuint> &indices)
{
	vec3 *tangents = new vec3[vertices.size()];
	vec3 *binormals = new vec3[vertices.size()];

	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		vec3 vertex0 = vertices[indices[i + 0]].position;
		vec3 vertex1 = vertices[indices[i + 1]].position;
		vec3 vertex2 = vertices[indices[i + 2]].position;

		vec3 normal = glm::cross(vertex1 - vertex0, vertex2 - vertex0);

		vec3 deltaPos;
		if (vertex0.x == vertex1.x && vertex0.y == vertex1.y && vertex0.z == vertex1.z)
		{
			deltaPos = vertex2 - vertex0;
		}
		else
		{
			deltaPos = vertex1 - vertex0;
		}

		vec2 uv0 = vertices[indices[i + 0]].texCoord;
		vec2 uv1 = vertices[indices[i + 1]].texCoord;
		vec2 uv2 = vertices[indices[i + 2]].texCoord;

		vec2 deltaUV1 = uv1 - uv0;
		vec2 deltaUV2 = uv2 - uv0;

		vec3 tan;
		vec3 bin;

		tan = deltaPos / 1.0f;
		tan = tan - (glm::dot(normal, tan) * normal);
		tan = glm::normalize(tan);

		bin = glm::cross(tan, normal);
		bin = glm::normalize(bin);

		tangents[indices[i + 0]] = tan;
		tangents[indices[i + 1]] = tan;
		tangents[indices[i + 2]] = tan;

		binormals[indices[i + 0]] = bin;
		binormals[indices[i + 1]] = bin;
		binormals[indices[i + 2]] = bin;
	}

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		vertices[i].tangent = tangents[i];
		vertices[i].binormal = binormals[i];
	}
}

vector <Material> Model::LoadMtl(string relativePath, string fileName)
{
	vector<Material> materialList;
	ifstream file;
	file.open(relativePath + "/" + fileName);
	if (file.is_open())
	{
		while (file.good())
		{
			string token;
			file >> token;
			if (token == "newmtl")
			{
				materialList.emplace_back();
				file >> materialList.back().name;
			}
			else if (token == "Ka")
			{
				vec3 _ambient;
				file >> _ambient.x >> _ambient.y >> _ambient.z;
				materialList.back().ambient = _ambient;
			}
			else if (token == "Kd")
			{
				vec3 _diffuse;
				file >> _diffuse.x >> _diffuse.y >> _diffuse.z;
				materialList.back().diffuse = _diffuse;
			}
			else if (token == "Ks")
			{
				vec3 _specular;
				file >> _specular.x >> _specular.y >> _specular.z;
				materialList.back().specular = _specular;
			}
			else if (token == "Ns")
			{
				file >> materialList.back().shininess;
			}
			else if (token == "d")
			{
				file >> materialList.back().transparency;
			}
			else if (token == "illum")
			{
				GLuint illumination;
				file >> illumination;
			}
			else if (token == "map_Ka")
			{
				string name;
				file >> name;
				Texture *_texture = new Texture(Bitmap::bitmapFromFile(relativePath + "/" + name));
				materialList.back().ambientMap = _texture;
			}
			else if (token == "map_Kd")
			{
				string name;
				file >> name;
				Texture* _texture = new Texture(Bitmap::bitmapFromFile(relativePath + "/" + name));
				materialList.back().diffuseMap = _texture;
			}
			else if (token == "map_Ks")
			{
				string name;
				file >> name;
				Texture* _texture = new Texture(Bitmap::bitmapFromFile(relativePath + "/" + name));
				materialList.back().specularMap = _texture;
			}
			else if (token == "map_Ns")
			{
				string name;
				file >> name;
				Texture* _texture = new Texture(Bitmap::bitmapFromFile(relativePath + "/" + name));
				materialList.back().shininessMap = _texture;
			}
			else if (token == "map_d")
			{
				string name;
				file >> name;
				Texture* _texture = new Texture(Bitmap::bitmapFromFile(relativePath + "/" + name));
				materialList.back().transparencyMap = _texture;
			}
			else if (token == "map_bump")
			{
				string name;
				file >> name;
				file >> name;
				file >> name;
				Texture* _texture = new Texture(Bitmap::bitmapFromFile(relativePath + "/" + name));
				materialList.back().normalMap = _texture;
			}
			file.ignore(INT_MAX, '\n');
		}
		file.close();
	}

	return materialList;
}

GLvoid Model::CreateBuffers(Program* program, Group& group, vector<Mesh::Vertex>& vertices, vector <GLuint>& indices)
{
	GLuint VBO, IBO;

	glGenVertexArrays(1, &group.VAO);
	glBindVertexArray(group.VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(program->attrib("vertPosition"));
	glVertexAttribPointer(program->attrib("vertPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (const GLvoid*)0);

	glEnableVertexAttribArray(program->attrib("vertTexCoord"));
	glVertexAttribPointer(program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (const GLvoid*)(sizeof(vec3)));

	glEnableVertexAttribArray(program->attrib("vertNormal"));
	glVertexAttribPointer(program->attrib("vertNormal"), 3, GL_FLOAT, GL_TRUE, sizeof(Mesh::Vertex), (const GLvoid*)(sizeof(vec3) + sizeof(vec2)));

	glEnableVertexAttribArray(program->attrib("vertTangent"));
	glVertexAttribPointer(program->attrib("vertTangent"), 3, GL_FLOAT, GL_TRUE, sizeof(Mesh::Vertex), (const GLvoid*)(sizeof(vec3) + sizeof(vec2) + sizeof(vec3)));

	glEnableVertexAttribArray(program->attrib("vertBinormal"));
	glVertexAttribPointer(program->attrib("vertBinormal"), 3, GL_FLOAT, GL_TRUE, sizeof(Mesh::Vertex), (const GLvoid*)(sizeof(vec3) + sizeof(vec2) + sizeof(vec3) + sizeof(vec3)));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	vertices.clear();
	indices.clear();
}

Collider Model::CalculateCollider(vec3& minimum, vec3& maximum)
{
	vec3 size = (maximum - minimum) * 0.5f;
	vec3 position = minimum + size;
	GLfloat radio = std::max(std::max(size.x, size.y), size.z);
	Collider _collider = Collider();
	_collider.aabb = AxisAlignedBoundingBox(minimum, maximum);
	_collider.bs = BoundingSphere(position, radio);
	_collider.bp = BoundingPoint(position);

	minimum = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	maximum = vec3(FLT_MIN, FLT_MIN, FLT_MIN);

	return _collider;
}