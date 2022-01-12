#include "Model.h"
#include "OBJ_Loader.h"

#include <iostream>

Model::Model(const char* modelName):
	VAO(0), VBO(0)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	std::cout << "Loading model " << modelName << "...\n";

	objl::Loader* loader = new objl::Loader();
	bool res = loader->LoadFile(modelName);

	if (!res) {
		std::cout << "Could not load model: " << modelName << "\nExiting ...\n";
		exit(0);
	}

	for (int i = 0; i < loader->LoadedVertices.size(); i++) {
		glm::vec3 vertex = glm::vec3(
			loader->LoadedVertices[i].Position.X,
			loader->LoadedVertices[i].Position.Y,
			loader->LoadedVertices[i].Position.Z
		);
		glm::vec2 texture = glm::vec2(
			loader->LoadedVertices[i].TextureCoordinate.X,
			loader->LoadedVertices[i].TextureCoordinate.Y
		);
		glm::vec3 normal = glm::vec3(
			loader->LoadedVertices[i].Normal.X,
			loader->LoadedVertices[i].Normal.Y,
			loader->LoadedVertices[i].Normal.Z
		);

		vertices.push_back(vertex);
		uvs.push_back(texture);
		normals.push_back(normal);
	}

	verticesCount = vertices.size();

	for (int i = 0; i < verticesCount; i++) {
		Vertices.push_back(vertices[i].x);
		Vertices.push_back(vertices[i].y);
		Vertices.push_back(vertices[i].z);
		Vertices.push_back(uvs[i].x);
		Vertices.push_back(uvs[i].y);
	}
}