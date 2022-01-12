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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height;
	unsigned char* image = SOIL_load_image(imageName, &width, &height, 0, SOIL_LOAD_RGB);
	if (image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
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
		instance->Obsz += instance->getObsIncrease();
		break;
	case '-':
		instance->Obsz -= instance->getObsIncrease();
		break;
	case 'a':
		instance->Refx += 20;
		break;
	case 'd':
		instance->Refx -= 20;
		break;
	case 'w':
		instance->Refy += 20;
		break;
	case 's':
		instance->Refy -= 20;
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
		instance->Obsx += instance->getObsIncrease();
		break;
	case GLUT_KEY_RIGHT:
		instance->Obsx -= instance->getObsIncrease();
		break;
	case GLUT_KEY_UP:
		instance->Obsy += instance->getObsIncrease();
		break;
	case GLUT_KEY_DOWN:
		instance->Obsy -= instance->getObsIncrease();
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
	//glfwInit();
}

void Scene::InitializeScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	Model* table = new Model("table.obj");
	Model* burger = new Model("burger.obj");

	models.push_back(table);
	models.push_back(burger);

	// Creare VBO+shader
	CreateVBO();
	CreateShaders("tex.vert", "tex.frag");

	LoadTexture(LavaTexture, "lava.png");
	LoadTexture(WoodTexture, "wood.png");
	LoadTexture(PlainTexture, "plain.png");

	// Locatii ptr shader
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "viewShader");
	projLocation = glGetUniformLocation(ProgramId, "projectionShader");

}

void Scene::CreateShaders(const char* vertShader, const char* fragShader) {

	ProgramId = LoadShaders(vertShader, fragShader);
	glUseProgram(ProgramId);
}

void Scene::RenderFunction() {
	glUseProgram(ProgramId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	Obs = glm::vec3(Obsx, Obsy, Obsz);
	PctRef = glm::vec3(Refx, Refy, Refz);
	Vert = glm::vec3(Vx, Vy, Vz);
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), znear, zfar);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	////////////////////////////////////////
	// /////////// TABLE
	////////////////////////////////////////
	int i = 0;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the table's position
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the table
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, WoodTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the table
	glDrawArrays(GL_TRIANGLES, 0, models[i]->verticesCount);

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// /////////// BURGER
	////////////////////////////////////////
	i++;
	glBindVertexArray(models[i]->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, models[i]->VAO);

	// set the table's position
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 6.6f, 0.0f));
	myMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1 / 30.0f, 1 / 30.0f, 1 / 30.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// new texture for the table
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PlainTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	// draw the table
	glDrawArrays(GL_TRIANGLES, 0, models[i]->Vertices.size());

	////////////////////////////////////////////////////////////////////////////////////////

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const GLvoid*)(3 * sizeof(float)));
	}
}

void Scene::DestroyShaders()
{
	glDeleteProgram(ProgramId);
}

void Scene::DestroyVBO()
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for (int i = 0; i < models.size(); i++) {
		glDeleteBuffers(1, &models[i]->VBO);
	}
}

float Scene::getObsIncrease() const {
	return obsIncrease;
}