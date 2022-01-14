#include "Scene.h"
#include "Model.h"
#include "loadShaders.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "SOIL/SOIL.h"

void LoadTexture(GLuint& texture, const char* imageName)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image(imageName, &width, &height, 0, SOIL_LOAD_RGB);
	if (image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR: Failed to load texture " << imageName << " - ";
		std::cout << SOIL_last_result() << "\n";
	}

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void renderCallback() {
	Scene* instance = Scene::getInstance();
	instance->RenderFunction();
}

void processNormalKeys(unsigned char key, int x, int y) {

	Scene* instance = Scene::getInstance();
	switch (key)
	{
	case 'l':
		instance->Vx += 0.2;
		break;
	case 'r':
		instance->Vx -= 0.2;
		break;
	case '+':
		instance->distance += instance->getDistanceIncrease();
		break;
	case '-':
		instance->distance -= instance->getDistanceIncrease();
		break;
	case 'a':
	case 'A':
		instance->Refx += 1;
		break;
	case 'd':
	case 'D':
		instance->Refx -= 1;
		break;
	case 'w':
	case 'W':
		instance->Refy += 1;
		break;
	case 's':
	case 'S':
		instance->Refy -= 1;
		break;
	}
	if (key == 27)
		exit(0);
}

void processSpecialKeys(int key, int xx, int yy) {
	Scene* instance = Scene::getInstance();
	switch (key)
	{
	case GLUT_KEY_LEFT:
		instance->beta += instance->getBetaIncrease();
		break;
	case GLUT_KEY_RIGHT:
		instance->beta -= instance->getBetaIncrease();
		break;
	case GLUT_KEY_UP:
		instance->alpha += instance->getFirstAlphaIncrease();
		if (abs(instance->alpha - instance->PI / 2) < 0.05)
		{
			instance->setFirstAlphaIncrease(0.0);
		}
		else
		{
			instance->setFirstAlphaIncrease(0.01);
		}
		break;
	case GLUT_KEY_DOWN:
		instance->alpha -= instance->getSecondAlphaIncrease();
		if (abs(instance->alpha - instance->PI / 2) < 0.05)
		{
			instance->setSecondAlphaIncrease(0.0);
		}
		else
		{
			instance->setSecondAlphaIncrease(0.01);
		}
		break;
	}
}

void cleanupCallback() {
	Scene* instance = Scene::getInstance();
	instance->DestroyShaders();
	instance->DestroyVBO();
}

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

	Model* table = new Model("table.obj");
	Model* chicken = new Model("chicken.obj");
	Model* plate = new Model("plate.obj");
	Model* book = new Model("book.obj");
	Model* cup = new Model("cup.obj");
	Model* glass = new Model("glass.obj");
	Model* saltPepper = new Model("salt-and-pepper.obj");
	Model* cutlery = new Model("cutlery.obj");
	Model* burger = new Model("burger.obj");
	Model* tableCloth = new Model("cloth.obj");
	Model* ground = new Model("ground.obj");

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

	LoadTexture(LavaTexture, "lava.png");
	LoadTexture(WoodTexture, "wood.png");
	LoadTexture(PlainTexture, "plain.png");
	LoadTexture(BookTexture, "go-set-a-watchman2.png");
	LoadTexture(BookTexture2, "themartian6.png");
	LoadTexture(PagesTexture, "page.png");
	LoadTexture(MarbleTexture, "marble.png");
	LoadTexture(ChickenTexture, "chicken.png");
	LoadTexture(ClothTexture, "cloth.png");

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "viewShader");
	projLocation = glGetUniformLocation(ProgramId, "projectionShader");
	lightColorLoc = glGetUniformLocation(ProgramId, "lightColor");
	lightPosLoc = glGetUniformLocation(ProgramId, "lightPos");
	viewPosLoc = glGetUniformLocation(ProgramId, "viewPos");
	shadowMatrixLocation = glGetUniformLocation(ProgramId, "shadowMatrix");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	shadowColorLoc = glGetUniformLocation(ProgramId, "shadowColor");
}

void Scene::CreateShaders(const char* vertShader, const char* fragShader) {

	ProgramId = LoadShaders(vertShader, fragShader);
	glUseProgram(ProgramId);
}
void Scene::EnableShadow() {
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glUniformMatrix4fv(shadowMatrixLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
}
void Scene::DisableShadow() {
	codCol = 0;
	glUniform1i(codColLocation, codCol);
}
void Scene::DrawShadow(float D, int i, glm::vec3 shadowColor) {
	ComputeShadowMatrix(D);
	EnableShadow();
	glUniform3f(shadowColorLoc, shadowColor.x, shadowColor.y, shadowColor.z);
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);
	DisableShadow();
}
void Scene::RenderFunction() {
	glUseProgram(ProgramId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	Obsx = Refx + distance * cos(alpha) * cos(beta);
	Obsy = Refy + distance * sin(alpha);
	Obsz = Refz + distance * cos(alpha) * sin(beta);

	DisableShadow();
	
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
	
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// TABLE CLOTH
	////////////////////////////////////////
	int i = 0;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.4f, -0.2f));
	myMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.6f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ClothTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	// draw the shadow
	DrawShadow(- FLOOR_Y, i, lightShadow);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// GROUND
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, -7.0f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f,1.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PlainTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// CHICKEN
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 6.6f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1 / 30.0f, 1 / 30.0f, 1 / 30.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ChickenTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	//draw the shadow
	DrawShadow( - TABLE_Y, i, darkShadow);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// BOOK
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	glm::mat4 resizeBook = glm::scale(glm::mat4(1.0f), glm::vec3(0.007, 0.007, 0.007));
	glm::mat4 translateBook = glm::translate(glm::mat4(1.0f), glm::vec3(4, 6.13, 0));
	myMatrix =  translateBook * resizeBook;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PagesTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	int coverMeshSize = models[i]->MeshVertices[2].size() / 8;
	int totalVerticesSize = models[i]->Vertices.size() / 8;

	//draw the book pages
	glDrawArrays(GL_TRIANGLES, 0, totalVerticesSize - coverMeshSize);

	// new texture for the book cover
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, BookTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the book cover
	glDrawArrays(GL_TRIANGLES, totalVerticesSize - coverMeshSize, coverMeshSize);

	//draw the shadow
	DrawShadow( - TABLE_Y, i, darkShadow);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// BOOK 2
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

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

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PagesTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	coverMeshSize = models[i]->MeshVertices[2].size() / 8;
	totalVerticesSize = models[i]->Vertices.size() / 8;

	//draw the book pages
	glDrawArrays(GL_TRIANGLES, 0, totalVerticesSize - coverMeshSize);

	// new texture for the book cover
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, BookTexture2);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the book cover
	glDrawArrays(GL_TRIANGLES, totalVerticesSize - coverMeshSize, coverMeshSize);

	//draw the shadow
	DrawShadow( - TABLE_Y, i, darkShadow);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// CUP
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	glm::mat4 resizeCup = glm::scale(glm::mat4(1.0f), glm::vec3(0.25, 0.25, 0.25));
	glm::mat4 translateCup = glm::translate(glm::mat4(1.0f), glm::vec3(-4.25, 6.44, -1.0));
	myMatrix = translateCup * resizeCup;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MarbleTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	//draw the shadow
	DrawShadow( - TABLE_Y, i, darkShadow);

	////////////////////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////
	// /////////// PLATE
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 6.1f, 0.0f));
	myMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1 / 4.0f, 1 / 4.0f, 1 / 4.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MarbleTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	//draw the shadow
	DrawShadow( - TABLE_Y, i, darkShadow);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// CUTLERY
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	myMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	myMatrix *= glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 6.20f, 2.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PlainTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	//draw the shadow
	DrawShadow( - TABLE_Y, i, darkShadow);

	/// TRANSPARENT OBJECTS
	
	glEnable(GL_BLEND);
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// GLASS
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set the glass's position
	glm::mat4 resizeGlass = glm::scale(glm::mat4(1.0f), glm::vec3(10, 10, 10));
	glm::mat4 translateGlass = glm::translate(glm::mat4(1.0f), glm::vec3(-4.25, TABLE_Y, -1.5));
	myMatrix = translateGlass * resizeGlass;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MarbleTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

	// draw the glass
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	// draw the shadow
	glDisable(GL_BLEND);
	DrawShadow( - TABLE_Y, i, darkShadow);
	glEnable(GL_BLEND);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// SALT AND PEPPER
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, TABLE_Y, 2.0f));
	myMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(7.0f, 7.0f, 7.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PlainTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	// draw the shadow
	glDisable(GL_BLEND);
	DrawShadow( - TABLE_Y, i, darkShadow);
	glEnable(GL_BLEND);

	///  END TRANSPARENT OBJECTS

	glDisable(GL_BLEND);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// TABLE
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the object's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, FLOOR_Y, 0.0f));
	myMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1 / 1.5f, 1 / 1.5f, 1 / 1.5f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the object
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, WoodTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the object
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	// draw the shadow
	DrawShadow(- FLOOR_Y, i, lightShadow);

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
void Scene::setFirstAlphaIncrease(float value) {
	this->firstAlphaIncrease = value;
}
void Scene::setSecondAlphaIncrease(float value) {
	this->secondAlphaIncrease = value;
}
