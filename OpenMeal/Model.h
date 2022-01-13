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
	int meshCount;

	std::vector<float> Vertices;
	std::vector<std::vector<float>> MeshVertices;

};
