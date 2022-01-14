#pragma once

#include "Model.h"

#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/gtc/quaternion.hpp"
#include "glm/glm/gtx/quaternion.hpp"

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
	int codCol;
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
	float shadowMatrix[4][4];

	float TABLE_Y = 6.13;
	float FLOOR_Y = - 0.25;

	// textures
	GLuint
		LavaTexture,
		WoodTexture,
		PlainTexture,
		BookTexture,
		BookTexture2,
		MarbleTexture,
		ChickenTexture,
		ClothTexture,
		PagesTexture;

	// for the shaders
	GLuint
		ProgramId,
		viewLocation,
		projLocation,
		myMatrixLocation,
		lightColorLoc, lightPosLoc, viewPosLoc, shadowColorLoc,
		shadowMatrixLocation, codColLocation;

	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightShadow = glm::vec3(0.66f, 0.66f, 0.66f);
	glm::vec3 darkShadow = glm::vec3(0.02f, 0.0625f, 0.0156f);
	glm::vec3 lightPosition = glm::vec3(-500.0f, 500.0f, -350.0f);

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
	void ComputeShadowMatrix(float);
	void EnableShadow();
	void DisableShadow();
	void DrawShadow(float, int, glm::vec3);
	void RenderFunction();
	void CreateVBO();
	void DestroyShaders();
	void DestroyVBO();

};
