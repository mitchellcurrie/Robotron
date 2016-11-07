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

// #include "FMOD\fmod.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "global.h"

static int windowID;

/***********************
* render: Updates the game, renders entities and renders text
* Parameters: none
* Return: void
********************/
void render() {
	GameScene::GetInstance().Update();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GameScene::GetInstance().RenderEntities(); // for entities
	GameScene::GetInstance().RenderText(); // for text
	glutSwapBuffers();
}

/***********************
* key down: gets keyboard input for pressing a key down
* Parameters: unsigned char for key pressed, integer x and y
* Return: void
********************/
void KeyDown(unsigned char key, int x, int y) {

	Entity::KeyDown(key, x, y);
	GameScene::GetInstance().KeyDown(key, x, y);
}

/***********************
* key up: gets keyboard input for releasing a key
* Parameters: unsigned char for key released, integer x and y
* Return: void
********************/
void KeyUp(unsigned char key, int x, int y) {
	Entity::KeyUp(key, x, y);
	GameScene::GetInstance().KeyUp(key, x, y);
}

/***********************
* main: main function of the game
* Parameters: integer argc, char ** argv
* Return: integer
********************/
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

