#pragma once

#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OBJ_Loader.h"

#include "glm/glm/glm.hpp"  

class Model {
private:


public:

	Model(const char*);

	GLuint
		VAO,
		VBO;

	int verticesCount;

	std::vector<float> Vertices;
	std::vector<objl::Mesh> Meshes;
	std::vector<std::vector<float>> MeshVertices;

};
