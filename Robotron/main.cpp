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

	Add power ups

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

float g_fDeltaTick;

void render()
{
	//srand(time(NULL));

	if (GameScene::GetInstance().IsLevelComplete())
	{
		GameScene::GetInstance().NextLevel();
		GameScene::GetInstance().CreateEntities();   // Create entities of the new level
		GameScene::GetInstance().SetLevelComplete(false);
	}
		

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	GameScene::GetInstance().ExecuteOneFrame();
	GameScene::GetInstance().GetClock()->Process();
	g_fDeltaTick = GameScene::GetInstance().GetClock()->GetDeltaTick();

	GameScene::GetInstance().SetPositions(g_fDeltaTick);
	GameScene::GetInstance().CheckCollisions();
	GameScene::GetInstance().CheckBullets();
	GameScene::GetInstance().CheckEntities();
	
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
	GLfloat width = 1600;
	GLfloat height = 900;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(250, 50);
	glutInitWindowSize(1600, 900);
	glutCreateWindow("Robotron");

	glewInit();

	Camera Camera(vec3(0.0f, 15.0f, 22.0f), vec3(0.0f, -0.9f, -1.0f), vec3(0.0f, 1.0f, 0.0f), width, height);
	//            Pos:  x      y      z    Front:       y tilt          Up:       

	GameScene::GetInstance().AddCamera(Camera);
	GameScene::GetInstance().CreateEntities();
	GameScene::GetInstance().CreateText();
	GameScene::GetInstance().GetClock()->Initialise();

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

