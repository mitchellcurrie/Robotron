//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : main.cpp
// Description : main implementation file
// Author : Juan Rodriguez & Mitchell Currie
// Mail : Juan.Rodriguez@mediadesignschool.com
//		  Mitchell.Currie@mediadesignschool.com
//        

/*

	To do:

	Text for extra Life - goes away after e.g. 5 kills or next level, or death
	Change multiple text to then switch off

	Bonus stuff:
	Power up text for when active - set colour to red when timer almost finished


*/

#include <iostream>
#include <vector>

#include "glew.h"
#include "freeglut.h"
#include "SOIL.h"
#include "TextLabel.h"
#include "model.h"
#include "camera.h"
#include "gameScene.h"
#include "ShaderLoader.h"

#include <ft2build.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "global.h"

static int windowID;

void render() {
	GameScene::GetInstance().Update();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GameScene::GetInstance().RenderEntities(); // for entities
	GameScene::GetInstance().RenderText(); // for text

	glutSwapBuffers();
}

void KeyDown(unsigned char key, int x, int y) {

	Entity::KeyDown(key, x, y);
	GameScene::GetInstance().KeyDown(key, x, y);
}

void KeyUp(unsigned char key, int x, int y) {
	Entity::KeyUp(key, x, y);
	GameScene::GetInstance().KeyUp(key, x, y);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(170, 35);
	glutInitWindowSize(WIDTH, HEIGHT);
	windowID = glutCreateWindow("Robotron");

	glewInit();

	GameScene::GetInstance().SetUp(WIDTH, HEIGHT);

	// glutIdleFunc(update);

	//clear
	glClearColor(0.0, 0.0, 1.0, 0.0);

	// register callbacks
	glutDisplayFunc(render);

	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);

	glutMainLoop();

	return 0;
}

