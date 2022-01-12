#include "Scene.h"
#include "loadShaders.h"
#include "objloader.hpp"

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
		instance->Obsz += 0.2;
		break;
	case '-':
		instance->Obsz -= 0.2;
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
		instance->Obsx -= 0.2;
		break;
	case GLUT_KEY_RIGHT:
		instance->Obsx += 0.2;
		break;
	case GLUT_KEY_UP:
		instance->Obsy -= 0.2;
		break;
	case GLUT_KEY_DOWN:
		instance->Obsy += 0.2;
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

	glutCreateWindow("Desenarea unui cub folosind testul de adancime");
	glewInit();

	glutDisplayFunc(renderCallback);
	glutIdleFunc(renderCallback);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(cleanupCallback);
	//glfwInit();
}

void Scene::InitializeScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului

	bool res = loadOBJ("Sfera.obj", vertices, uvs, normals);
	bool res2 = loadOBJ("cube.obj", cubeVertices, cubeUvs, cubeNormals);

	for (int i = 0; i < vertices.size(); i++) {
		Vertices.push_back(vertices[i].x);
		Vertices.push_back(vertices[i].y);
		Vertices.push_back(vertices[i].z);
		Vertices.push_back(uvs[i].x);
		Vertices.push_back(uvs[i].y);
	}

	for (int i = 0; i < cubeVertices.size(); i++) {
		CubeVertices.push_back(cubeVertices[i].x);
		CubeVertices.push_back(cubeVertices[i].y);
		CubeVertices.push_back(cubeVertices[i].z);
		CubeVertices.push_back(cubeUvs[i].x);
		CubeVertices.push_back(cubeUvs[i].y);
	}

	//cout << vertices.size() << " " << Vertices.size() << " \n\n\n";

	// Creare VBO+shader
	CreateVBO();
	CreateShaders("tex.vert", "tex.frag");

	LoadTexture(Texture, "green_circle.png");

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

	//CreateVBO(); // decomentati acest rand daca este cazul 
	glBindVertexArray(VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// matricea de vizualizare
	Obs = glm::vec3(Obsx, Obsy, Obsz); // pozitia observatorului	
	Refx = Obsx; Refy = Obsy;
	PctRef = glm::vec3(Refx, Refy, 800); // pozitia punctului de referinta
	Vert = glm::vec3(Vx, 1.0f, 0.0f); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// matricea de proiectie, pot fi testate si alte variante
	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), znear, zfar);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);


	// Fetele
	glDrawArrays(GL_TRIANGLES, 0, 4104);

	glBindVertexArray(CubeVao);

	myMatrix = glm::translate(glm::vec3(-5, 0, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	//glBindBuffer(GL_ARRAY_BUFFER, CubeVboId);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
	glFlush();
}

void Scene::CreateVBO() {
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	// generare VAO/buffere
	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), &Vertices[0], GL_STATIC_DRAW);
	
	// se activeaza lucrul cu atribute; 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const GLvoid*)(3 * sizeof(float)));

	glGenVertexArrays(1, &CubeVao);
	glBindVertexArray(CubeVao);

	glGenBuffers(1, &CubeVboId);
	glBindBuffer(GL_ARRAY_BUFFER, CubeVboId);
	glBufferData(GL_ARRAY_BUFFER, CubeVertices.size() * sizeof(float), &CubeVertices[0], GL_STATIC_DRAW);


	// se activeaza lucrul cu atribute; 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const GLvoid*)(3 * sizeof(float)));

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
	glDeleteBuffers(1, &VboId);
}