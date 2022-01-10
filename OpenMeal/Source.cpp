/*
- Folosirea indexarii pentru a trasa separata fetele si muchiile unui obiect 3D (cub)
- Rolul testului de adancime
*/
#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h
#include "loadShaders.h"
#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "objloader.hpp"
#include "SOIL/SOIL.h"

using namespace std;

// identificatori 
GLuint
	VaoId,
	VboId,
	EboId,
	Texture,
	ProgramId,
	viewLocation,
	projLocation,
	codColLocation; 

std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;

// variabile
int codCol;

// elemente pentru matricea de vizualizare
float Obsx = 0.0, Obsy = 0.0, Obsz = -10.f;
float Refx = 0.0f, Refy = 0.0f;
float Vx = 0.0;

// elemente pentru matricea de proiectie
float width = 800, height = 600, xwmin = -200.f, xwmax = 200, ywmin = -200, ywmax = 200, znear = 1, zfar = -100, fov = 30;

// vectori
glm::vec3 Obs, PctRef, Vert;

// matrice utilizate
glm::mat4 view, projection;

#define	checkImageWidth 64
#define	checkImageHeight 64

static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

void makeCheckImage(void)
{
	int i, j, c;
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 200;

			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}

void LoadTexture(GLuint& texture, const char* imageName)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height;
	unsigned char* image = SOIL_load_image(imageName, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'l':
		Vx += 0.1;
		break;
	case 'r':
		Vx -= 0.1;
		break;
	case '+':
		Obsz += 10;
		break;
	case '-':
		Obsz -= 10;
		break;
	}
	if (key == 27)
		exit(0);
}
void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Obsx -= 0.1;
		break;
	case GLUT_KEY_RIGHT:
		Obsx += 0.1;
		break;
	case GLUT_KEY_UP:
		Obsy += 0.1;
		break;
	case GLUT_KEY_DOWN:
		Obsy -= 0.1;
		break;
	}
}
void CreateVBO(void)
{
	// varfurile 
	//GLfloat Vertices[] =
	//{
	//	// varfurile din planul z=-50  
	//	// coordonate                   // culori			
	//	-50.0f,  -50.0f, -50.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	//	50.0f,  -50.0f,  -50.0f, 1.0f,   0.0f, 0.9f, 0.0f,
	//	50.0f,  50.0f,  -50.0f, 1.0f,    0.0f, 0.6f, 0.0f,
	//	-50.0f,  50.0f, -50.0f, 1.0f,   0.0f, 0.2f, 0.0f,
	//	// varfurile din planul z=+50  
	//	// coordonate                   // culori			
	//	-50.0f,  -50.0f, 50.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	//	50.0f,  -50.0f,  50.0f, 1.0f,   0.7f, 0.0f, 0.0f,
	//	50.0f,  50.0f,  50.0f, 1.0f,    0.5f, 0.0f, 0.0f,
	//	-50.0f,  50.0f, 50.0f, 1.0f,   0.1f, 0.0f, 0.0f,
	//};

	//// indicii pentru varfuri
	//GLubyte Indices[] =
	//{
	//	1, 0, 2,   2, 0, 3,  //  Fata "de jos"
	//	2, 3, 6,   6, 3, 7,  // Lateral 
	//	7, 3, 4,   4, 3, 0,  // Lateral 
	//	4, 0, 5,   5, 0, 1,  // Lateral 
	//	1, 2, 5,   5, 2, 6,  // Lateral 
	//	5, 6, 4,   4, 6, 7, //  Fata "de sus"
	//	0, 1, 2, 3,  // Contur fata de jos
	//	4, 5, 6, 7,  // Contur fata de sus
	//	0, 4, // Muchie laterala
	//	1, 5, // Muchie laterala
	//	2, 6, // Muchie laterala
	//	3, 7 // Muchie laterala
	//};

	//// se creeaza un buffer nou
	//glGenBuffers(1, &VboId);
	//// este setat ca buffer curent
	//glBindBuffer(GL_ARRAY_BUFFER, VboId);
	//// punctele sunt "copiate" in bufferul curent
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	//// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	//glGenVertexArrays(1, &VaoId);
	//glBindVertexArray(VaoId);
	//// se activeaza lucrul cu atribute; atributul 0 = pozitie
	//glEnableVertexAttribArray(0);
	//// 
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//// un nou buffer, pentru culoare
	//glGenBuffers(1, &ColorBufferId);
	//glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	//// atributul 1 =  culoare
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);



	// generare VAO/buffere
	glGenBuffers(1, &VboId); // atribute
	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + uvs.size() * sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), uvs.size() * sizeof(glm::vec2), &uvs[0]);

	// se activeaza lucrul cu atribute; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vertices.size() * sizeof(glm::vec3)));


	/*glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(vf_pos));*/
}
void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);
}
void CreateShaders(void)
{
	//ProgramId = LoadShaders("08_01_Shader.vert", "08_01_Shader.frag");
	ProgramId = LoadShaders("tex.vert", "tex.frag");
	glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului

	bool res = loadOBJ("cube.obj", vertices, uvs, normals);
	cout << res << "\n";
	cout << vertices.size() << "\n";
	for (int i = 0; i < vertices.size(); i++) {
		cout << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";
	}

	// Creare VBO+shader
	CreateVBO();
	CreateShaders();

	LoadTexture(Texture, "texture.png");

	// Locatii ptr shader
	viewLocation = glGetUniformLocation(ProgramId, "viewShader");
	projLocation = glGetUniformLocation(ProgramId, "projectionShader");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");

}
void RenderFunction(void)
{
	glUseProgram(ProgramId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	// CreateVBO(); // decomentati acest rand daca este cazul 
	glBindVertexArray(VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);	

	// matricea de vizualizare
	Obs = glm::vec3(Obsx, Obsy, Obsz); // pozitia observatorului	
	Refx = Obsx; Refy = Obsy;
	PctRef = glm::vec3(Refx, Refy, 800); // pozitia punctului de referinta
	Vert = glm::vec3(Vx, 1.0f, 0.0f); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// matricea de proiectie, pot fi testate si alte variante
	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), znear, zfar);
	// projection = glm::ortho(xwmin, xwmax, ywmin, ywmax, znear, zfar);
	// projection = glm::frustum(xwmin, xwmax, ywmin, ywmax, 80.f, -80.f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	// Fetele
	codCol = 0;
	glLineWidth(3.5);
	glPointSize(5);
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POINTS, 0, 36);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// Muchiile
	//codCol = 1;
	//glUniform1i(codColLocation, codCol);
	//glLineWidth(3.5);
	//glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(36));
	//glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(40));
	//glDrawElements(GL_LINES, 8, GL_UNSIGNED_BYTE, (void*)(44));

	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
	glFlush();
}


void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1200, 900);
	glutCreateWindow("Desenarea unui cub folosind testul de adancime");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

