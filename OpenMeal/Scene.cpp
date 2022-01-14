#include "Scene.h"
#include "Model.h"
#include "helpers.h"
#include "LIBRARIES/loadShaders.h"
#include "LIBRARIES/LoadTexture.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LIBRARIES/glm/glm/glm.hpp"  
#include "LIBRARIES/glm/glm/gtc/matrix_transform.hpp"
#include "LIBRARIES/glm/glm/gtx/transform.hpp"
#include "LIBRARIES/glm/glm/gtc/type_ptr.hpp"

Scene* Scene::instance = nullptr;

Scene::Scene() {
	InitializeLibraries();
}

Scene* Scene::getInstance() {
	if (instance == nullptr) {
		instance = new Scene();
	}
	return instance;
}

void Scene::InitializeLibraries() {
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1200, 900);

	glutInitContextVersion(4, 4);

	glutCreateWindow("OpenMeal");
	glewInit();

	glutDisplayFunc(renderCallback);
	glutIdleFunc(renderCallback);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(cleanupCallback);
}

void Scene::ComputeShadowMatrix(float D) {
	shadowMatrix[0][0] = lightPosition.y + D;
	shadowMatrix[0][1] = 0;
	shadowMatrix[0][2] = 0;
	shadowMatrix[0][3] = 0;

	shadowMatrix[1][0] = -lightPosition.x;
	shadowMatrix[1][1] = D;
	shadowMatrix[1][2] = -lightPosition.z;
	shadowMatrix[1][3] = -1;

	shadowMatrix[2][0] = 0;
	shadowMatrix[2][1] = 0;
	shadowMatrix[2][2] = lightPosition.y + D;
	shadowMatrix[2][3] = 0;

	shadowMatrix[3][0] = -D * lightPosition.x;
	shadowMatrix[3][1] = -D * lightPosition.y;
	shadowMatrix[3][2] = -D * lightPosition.z;
	shadowMatrix[3][3] = lightPosition.y;
}

void Scene::InitializeScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	Model* table = new Model("Assets/OBJ/table.obj");
	Model* chicken = new Model("Assets/OBJ/chicken.obj");
	Model* plate = new Model("Assets/OBJ/plate.obj");
	Model* book = new Model("Assets/OBJ/book.obj");
	Model* cup = new Model("Assets/OBJ/cup.obj");
	Model* glass = new Model("Assets/OBJ/glass.obj");
	Model* saltPepper = new Model("Assets/OBJ/salt-and-pepper.obj");
	Model* cutlery = new Model("Assets/OBJ/cutlery.obj");
	Model* burger = new Model("Assets/OBJ/burger.obj");
	Model* tableCloth = new Model("Assets/OBJ/cloth.obj");
	Model* ground = new Model("Assets/OBJ/ground.obj");

	models.push_back(tableCloth);
	models.push_back(ground);
	models.push_back(chicken);
	models.push_back(book);
	models.push_back(book);
	models.push_back(cup);
	models.push_back(plate);
	models.push_back(cutlery);
	models.push_back(glass);
	models.push_back(saltPepper);
	models.push_back(table);

	CreateVBO();
	CreateShaders("tex.vert", "tex.frag");

	LoadTexture(WoodTexture, "Assets/Textures/wood.png");
	LoadTexture(PlainTexture, "Assets/Textures/plain.png");
	LoadTexture(BookTexture, "Assets/Textures/go-set-a-watchman2.png");
	LoadTexture(BookTexture2, "Assets/Textures/themartian6.png");
	LoadTexture(PagesTexture, "Assets/Textures/page.png");
	LoadTexture(MarbleTexture, "Assets/Textures/marble.png");
	LoadTexture(ChickenTexture, "Assets/Textures/chicken.png");
	LoadTexture(ClothTexture, "Assets/Textures/cloth.png");

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "viewShader");
	projLocation = glGetUniformLocation(ProgramId, "projectionShader");
	lightColorLoc = glGetUniformLocation(ProgramId, "lightColor");
	lightPosLoc = glGetUniformLocation(ProgramId, "lightPos");
	viewPosLoc = glGetUniformLocation(ProgramId, "viewPos");
	shadowMatrixLocation = glGetUniformLocation(ProgramId, "shadowMatrix");
	isShadowLocation = glGetUniformLocation(ProgramId, "isShadow");
	shadowColorLoc = glGetUniformLocation(ProgramId, "shadowColor");
}

void Scene::CreateShaders(const char* vertShader, const char* fragShader) {
	ProgramId = LoadShaders(vertShader, fragShader);
	glUseProgram(ProgramId);
}

void Scene::EnableShadow() {
	isShadow = true;
	glUniform1i(isShadowLocation, isShadow);
	glUniformMatrix4fv(shadowMatrixLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
}

void Scene::DisableShadow() {
	isShadow = false;
	glUniform1i(isShadowLocation, isShadow);
}

void Scene::DrawShadow(float D, int i, glm::vec3 shadowColor) {
	ComputeShadowMatrix(D);
	EnableShadow();
	glUniform3f(shadowColorLoc, shadowColor.x, shadowColor.y, shadowColor.z);
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);
	DisableShadow();
}

void Scene::DrawObject(int i, GLuint Texture) {
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);
}

void Scene::DrawMesh(int i, int start, int count, GLuint Texture) {
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	glDrawArrays(GL_TRIANGLES, start, count);
}

void Scene::CalculatePositionVariables() {
	Obsx = Refx + distance * cos(alpha) * cos(beta);
	Obsy = Refy + distance * sin(alpha);
	Obsz = Refz + distance * cos(alpha) * sin(beta);

	Obs = glm::vec3(Obsx, Obsy, Obsz);
	PctRef = glm::vec3(Refx, Refy, Refz);
	Vert = glm::vec3(Vx, Vy, Vz);
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glUniform3f(lightColorLoc, lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPosLoc, Obsx, Obsy, Obsz);

	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), znear, zfar);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
}

void Scene::RenderFunction() {
	glUseProgram(ProgramId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	DisableShadow();

	CalculatePositionVariables();
	
	////////////////////////////////////////
	// /////////// TABLE CLOTH
	////////////////////////////////////////
	int i = 0;

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.4f, -0.2f));
	myMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.6f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	DrawObject(i, ClothTexture);
	DrawShadow(- FLOOR_Y, i, lightShadow);

	////////////////////////////////////////
	// /////////// GROUND
	////////////////////////////////////////
	i++;

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, -7.0f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f,1.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	DrawObject(i, PlainTexture);

	////////////////////////////////////////
	// /////////// CHICKEN
	////////////////////////////////////////
	i++;

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 6.6f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1 / 30.0f, 1 / 30.0f, 1 / 30.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	DrawObject(i, ChickenTexture);
	DrawShadow(-TABLE_Y, i, darkShadow);

	////////////////////////////////////////
	// /////////// BOOK
	////////////////////////////////////////
	i++;

	// set the object's position
	glm::mat4 resizeBook = glm::scale(glm::mat4(1.0f), glm::vec3(0.007, 0.007, 0.007));
	glm::mat4 translateBook = glm::translate(glm::mat4(1.0f), glm::vec3(4, 6.13, 0));
	myMatrix = translateBook * resizeBook;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	int coverMeshSize = models[i]->MeshVertices[2].size() / 8;
	int totalVerticesSize = models[i]->Vertices.size() / 8;

	DrawMesh(i, 0, totalVerticesSize - coverMeshSize, PagesTexture);
	DrawMesh(i, totalVerticesSize - coverMeshSize, coverMeshSize, BookTexture);
	DrawShadow(-TABLE_Y, i, darkShadow);

	////////////////////////////////////////
	// /////////// BOOK 2
	////////////////////////////////////////
	i++;

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(4, 6.43, 1));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.007, 0.007, 0.007));
	glm::vec3 firstAxis = glm::vec3(1, 0, 0);
	glm::vec3 secondAxis = glm::vec3(0, 1, 0);
	glm::quat firstRotation = glm::angleAxis(glm::radians(11.0f), firstAxis);
	glm::quat secondRotation = glm::angleAxis(glm::radians(45.0f), secondAxis);
	glm::quat combinedRotation = firstRotation * secondRotation;
	glm::mat4 rotateBook2 = toMat4(combinedRotation);
	myMatrix *= rotateBook2;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	coverMeshSize = models[i]->MeshVertices[2].size() / 8;
	totalVerticesSize = models[i]->Vertices.size() / 8;
	
	DrawMesh(i, 0, totalVerticesSize - coverMeshSize, PagesTexture);
	DrawMesh(i, totalVerticesSize - coverMeshSize, coverMeshSize, BookTexture2);
	DrawShadow(-TABLE_Y, i, darkShadow);

	////////////////////////////////////////
	// /////////// CUP
	////////////////////////////////////////
	i++;

	// set the object's position
	glm::mat4 resizeCup = glm::scale(glm::mat4(1.0f), glm::vec3(0.25, 0.25, 0.25));
	glm::mat4 translateCup = glm::translate(glm::mat4(1.0f), glm::vec3(-4.25, 6.44, -1.0));
	myMatrix = translateCup * resizeCup;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	DrawObject(i, MarbleTexture);
	DrawShadow(-TABLE_Y, i, darkShadow);

	////////////////////////////////////////
	// /////////// PLATE
	////////////////////////////////////////
	i++;

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 6.1f, 0.0f));
	myMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1 / 4.0f, 1 / 4.0f, 1 / 4.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	DrawObject(i, MarbleTexture);
	DrawShadow(-TABLE_Y, i, darkShadow);

	////////////////////////////////////////
	// /////////// CUTLERY
	////////////////////////////////////////
	i++;

	// set the object's position
	myMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	myMatrix *= glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 6.20f, 2.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	DrawObject(i, PlainTexture);
	DrawShadow(-TABLE_Y, i, darkShadow);

	/// TRANSPARENT OBJECTS
	
	glEnable(GL_BLEND);

	////////////////////////////////////////
	// /////////// GLASS
	////////////////////////////////////////
	i++;

	// set the glass's position
	glm::mat4 resizeGlass = glm::scale(glm::mat4(1.0f), glm::vec3(10, 10, 10));
	glm::mat4 translateGlass = glm::translate(glm::mat4(1.0f), glm::vec3(-4.25, TABLE_Y, -1.5));
	myMatrix = translateGlass * resizeGlass;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

	DrawObject(i, MarbleTexture);

	// draw the shadow
	glDisable(GL_BLEND);
	DrawShadow(-TABLE_Y, i, darkShadow);
	glEnable(GL_BLEND);

	////////////////////////////////////////
	// /////////// SALT AND PEPPER
	////////////////////////////////////////
	i++;

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, TABLE_Y, 2.0f));
	myMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(7.0f, 7.0f, 7.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	DrawObject(i, PlainTexture);

	// draw the shadow
	glDisable(GL_BLEND);
	DrawShadow(-TABLE_Y, i, darkShadow);
	glEnable(GL_BLEND);

	///  END TRANSPARENT OBJECTS

	glDisable(GL_BLEND);

	////////////////////////////////////////
	// /////////// TABLE
	////////////////////////////////////////
	i++;

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, FLOOR_Y, 0.0f));
	myMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1 / 1.5f, 1 / 1.5f, 1 / 1.5f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	DrawObject(i, WoodTexture);
	DrawShadow(-FLOOR_Y, i, lightShadow);

	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
	glFlush();
}

void Scene::CreateVBO() {
	for (int i = 0; i < models.size(); i++) {
		glGenVertexArrays(1, &models[i]->VAO);
		glBindVertexArray(models[i]->VAO);

		glGenBuffers(1, &models[i]->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, models[i]->VBO);
		glBufferData(GL_ARRAY_BUFFER, models[i]->Vertices.size() * sizeof(float), &models[i]->Vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid*)(5 * sizeof(float)));
	}
}

void Scene::DestroyShaders()
{
	glDeleteProgram(ProgramId);
}

void Scene::DestroyVBO()
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for (int i = 0; i < models.size(); i++) {
		glDeleteBuffers(1, &models[i]->VBO);
	}
}

// getters

float Scene::getDistanceIncrease() const {
	return distanceIncrease;
}

float Scene::getFirstAlphaIncrease() const {
	return firstAlphaIncrease;
}

float Scene::getSecondAlphaIncrease() const {
	return secondAlphaIncrease;
}

float Scene::getBetaIncrease() const {
	return betaIncrease;
}

float Scene::getVx() const {
	return Vx;
}

float Scene::getVy() const {
	return Vy;
}

float Scene::getVz() const {
	return Vz;
}

float Scene::getDistance() const {
	return distance;
}

float Scene::getRefx() const {
	return Refx;
}

float Scene::getRefy() const {
	return Refy;
}

float Scene::getRefz() const {
	return Refz;
}

float Scene::getAlpha() const {
	return alpha;
}

float Scene::getBeta() const {
	return beta;
}

float Scene::getPI() const {
	return PI;
}

// setters

void Scene::setVx(float x) {
	Vx = x;
}

void Scene::setVy(float y) {
	Vy = y;
}

void Scene::setVz(float z) {
	Vz = z;
}

void Scene::setFirstAlphaIncrease(float value) {
	this->firstAlphaIncrease = value;
}

void Scene::setSecondAlphaIncrease(float value) {
	this->secondAlphaIncrease = value;
}

void Scene::setDistance(float d) {
	distance = d;
}

void Scene::setRefx(float x) {
	Refx = x;
}

void Scene::setRefy(float y) {
	Refy = y;
}

void Scene::setRefz(float z) {
	Refz = z;
}

void Scene::setAlpha(float a) {
	alpha = a;
}

void Scene::setBeta(float b) {
	beta = b;
}
