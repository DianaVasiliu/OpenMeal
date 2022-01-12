#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Scene.h"

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	Scene* scene = Scene::getInstance();
	scene->InitializeScene();

	glutMainLoop();
}
