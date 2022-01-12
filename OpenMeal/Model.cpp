#include "Model.h"
#include "objloader.hpp"

#include <iostream>

Model::Model(const char* modelName):
	VAO(0), VBO(0)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	bool res = loadOBJ(modelName, vertices, uvs, normals);

	verticesCount = vertices.size();

	for (int i = 0; i < verticesCount; i++) {
		Vertices.push_back(vertices[i].x);
		Vertices.push_back(vertices[i].y);
		Vertices.push_back(vertices[i].z);
		Vertices.push_back(uvs[i].x);
		Vertices.push_back(uvs[i].y);
	}

}