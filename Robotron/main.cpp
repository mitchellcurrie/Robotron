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

void render()
{
	GameScene::GetInstance().Update();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	GameScene::GetInstance().RenderEntities(); // for entities
	GameScene::GetInstance().RenderText(); // for text

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	Entity::entityKeyboard(key, x, y);
}

void keyboard_up(unsigned char key, int x, int y)
{
	Entity::entityKeyboard_up(key, x, y);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(170, 35);
	glutInitWindowSize(1600, 900);
	glutCreateWindow("Robotron");

	glewInit();

	GameScene::GetInstance().SetUp();

	// glutIdleFunc(update);

	//clear
	glClearColor(0.0, 0.0, 1.0, 0.0);

	// register callbacks
	glutDisplayFunc(render);

	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard_up);
	
	glutMainLoop();

	return 0;
}

