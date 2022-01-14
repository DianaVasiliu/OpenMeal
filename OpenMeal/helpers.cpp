#include "helpers.h"
#include "Scene.h"

void renderCallback() {
	Scene* instance = Scene::getInstance();
	instance->RenderFunction();
}

void processNormalKeys(unsigned char key, int x, int y) {

	Scene* instance = Scene::getInstance();
	switch (key)
	{
	case 'l':
		instance->setVx(instance->getVx() + 0.2);
		break;
	case 'r':
		instance->setVx(instance->getVx() - 0.2);
		break;
	case '+':
		instance->setDistance(instance->getDistance() + instance->getDistanceIncrease());
		break;
	case '-':
		instance->setDistance(instance->getDistance() - instance->getDistanceIncrease());
		break;
	case 'a':
	case 'A':
		instance->setRefx(instance->getRefx() + 1);
		break;
	case 'd':
	case 'D':
		instance->setRefx(instance->getRefx() - 1);
		break;
	case 'w':
	case 'W':
		instance->setRefy(instance->getRefy() + 1);
		break;
	case 's':
	case 'S':
		instance->setRefy(instance->getRefy() - 1);
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
		instance->setBeta(instance->getBeta() + instance->getBetaIncrease());
		break;
	case GLUT_KEY_RIGHT:
		instance->setBeta(instance->getBeta() - instance->getBetaIncrease());
		break;
	case GLUT_KEY_UP:
		instance->setAlpha(instance->getAlpha() + instance->getFirstAlphaIncrease());
		if (abs(instance->getAlpha() - instance->getPI() / 2) < 0.05)
		{
			instance->setFirstAlphaIncrease(0.0);
		}
		else
		{
			instance->setFirstAlphaIncrease(0.01);
		}
		break;
	case GLUT_KEY_DOWN:
		instance->setAlpha(instance->getAlpha() - instance->getSecondAlphaIncrease());
		if (abs(instance->getAlpha() - instance->getPI() / 2) < 0.05)
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
