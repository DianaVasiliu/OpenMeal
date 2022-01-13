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

	float distanceIncrease = 0.5f;
	float betaIncrease = 0.01;
	float firstAlphaIncrease = 0.01;
	float secondAlphaIncrease = 0.01;

public:
	float const PI = 3.141592;

	float Obsx = 0.0, Obsy = -100.0, Obsz = -10;
	float Refx = 0.0f, Refy = 10.0f, Refz = 0.0f;
	float Vx = 0.0, Vy = -1.0, Vz = 0.0;
	
	float 
		width = 800, 
		height = 600, 
		xwmin = -200.f, xwmax = 200, 
		ywmin = -200, ywmax = 200, 
		znear = 1, zfar = -100, 
		fov = 30;
	float alpha = 0.3f, beta = -83.25f, distance = 10.0f;

	glm::vec3 Obs, PctRef, Vert;
	glm::mat4 view, projection, myMatrix;

	// textures
	GLuint
		LavaTexture,
		WoodTexture,
		PlainTexture,
		BookTexture,
		BookTexture2,
		MarbleTexture,
		ChickenTexture,
		PagesTexture;

	// for the shaders
	GLuint
		ProgramId,
		viewLocation,
		projLocation,
		myMatrixLocation,
		lightColorLoc, lightPosLoc, viewPosLoc;

	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPosition = glm::vec3(-1000.0f, 1000.0f, -1000.0f);

	std::vector<Model*> models;

	static Scene* getInstance();
	float getDistanceIncrease() const;
	float getBetaIncrease() const;
	float getFirstAlphaIncrease() const;
	float getSecondAlphaIncrease() const;
	void setFirstAlphaIncrease(float);
	void setSecondAlphaIncrease(float);

	void InitializeLibraries();
	void InitializeScene();
	void CreateShaders(const char*, const char*);
	void RenderFunction();
	void CreateVBO();
	void DestroyShaders();
	void DestroyVBO();

};
