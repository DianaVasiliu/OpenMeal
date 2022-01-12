#pragma once

#include "Model.h"

#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

class Scene {

private:
	Scene();
	static Scene* instance;

public:
	// elemente pentru matricea de vizualizare
	float Obsx = 0.0, Obsy = 0.0, Obsz = -25;
	float Refx = 0.0f, Refy = 0.0f;
	float Vx = 0.0;
	
	// elemente pentru matricea de proiectie
	float width = 800, height = 600, xwmin = -200.f, xwmax = 200, ywmin = -200, ywmax = 200, znear = 1, zfar = -100, fov = 30;

	// vectori
	glm::vec3 Obs, PctRef, Vert;

	// matrice utilizate
	glm::mat4 view, projection, myMatrix;

	GLuint
		GreenCircleTexture,
		LavaTexture,
		ProgramId,
		viewLocation,
		projLocation,
		myMatrixLocation;

	std::vector<Model*> models;

	static Scene* getInstance();

	void InitializeLibraries();
	void InitializeScene();
	void CreateShaders(const char*, const char*);
	void RenderFunction();
	void CreateVBO();
	void DestroyShaders();
	void DestroyVBO();

};
