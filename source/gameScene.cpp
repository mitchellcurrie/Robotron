//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : gameScene.cpp
// Description : Game scene implementation file 
// Mail : Mitchell.Currie@mediadesignschool.com
//		  Juan.Rodriguez@mediadesignschool.com
//

#include <iostream>

#include <Windows.h>
#include <cassert>
//#include <thread>
#include <ctime>

#include "consoletools.h"
#include "utils.h"
//#include "network.h"
//#include "client.h"
//#include "server.h"
#include "InputLineBuffer.h"
#include <functional>

#include "glew.h"
#include "freeglut.h"

#include "gameScene.h"

// make sure the winsock lib is included...
#pragma comment(lib,"ws2_32.lib")

// Static variables.
GameScene* GameScene::s_pGameScene{ 0 };
std::clock_t GameScene::m_startAITimer = std::clock();
double GameScene::m_durationAITimer = 0.0f;
std::clock_t GameScene::m_startSpeedBoostTimer = std::clock();
double GameScene::m_durationSpeedBoostTimer = 0.0f;
std::clock_t GameScene::m_startFastFireTimer = std::clock();
double GameScene::m_durationFastFireTimer = 0.0f;
std::clock_t GameScene::m_startEnemyFleeTimer = std::clock();
double GameScene::m_durationEnemyFleeTimer = 0.0f;
int GameScene::m_iExtraEnemies = 0;

// MENU
int GameScene::m_iCursorIndex = 0;
const int MENU_CHOICES = 3;   // changed from CHOICES

// Keyboard input
unsigned char GameScene::keyState[255];

// Networking
const float KEEPALIVE_INTERVAL = 0.5f;
int GameScene::m_iClientChoices = 0;

GameScene::GameScene() :
	m_iWidth(0),
	m_iHeight(0),
	m_iCurrentLevel(1),
	m_bLevelComplete(false),
	m_iPlayerLives(3),
	m_fDeltaTick(0.0f),
	m_iScore(0),
	m_iScoreCounter(0) {

	m_iGameState = MENU;

	m_pClock = new CClock;
	m_pBullet = nullptr;
	m_pPlayer1 = nullptr;
	m_pPlayer2 = nullptr;
	m_pPlayer3 = nullptr;
	m_pPlayer4 = nullptr;
	m_pEnemy = nullptr;
	m_pMap = nullptr;
	m_pTextLabel = nullptr;
	m_pPowerUp = nullptr;

	m_pAudioMgr = nullptr;

	m_pBGMusic = nullptr;
	m_pDieSound = nullptr;
	m_pHitSound = nullptr;
	m_pPowerUpSound = nullptr;
	m_pSelectSound = nullptr;
	m_pShootSound = nullptr;
	m_pScrollSound = nullptr;

	m_pBGChannel = nullptr;
	m_pDieChannel = nullptr;
	m_pHitChannel = nullptr;
	m_pPowerUpChannel = nullptr;
	m_pSelectChannel = nullptr;
	m_pShootChannel = nullptr;
	m_pScrollChannel = nullptr;

	m_pNetwork = nullptr;
	m_pClient = nullptr;
	m_pServer = nullptr;

}

GameScene::~GameScene() {
	delete m_pClock;
	m_pClock = 0;
}

GameScene& GameScene::GetInstance() {

	if (s_pGameScene == 0) {
		s_pGameScene = new GameScene();
	}

	return (*s_pGameScene);

}

#pragma region CREATE
/***********************
* GameScene set up: Creates the camera and calls all of the game set up functions
* Parameters: integers width and height for the screen size
* Return: void
********************/
void GameScene::SetUp(int _iWidth, int _iHeight) {

	m_iWidth = _iWidth;
	m_iHeight = _iHeight;

	//	Camera Camera(vec3(0.0f, 15.0f, 22.0f), vec3(0.0f, -0.9f, -1.0f), vec3(0.0f, 1.0f, 0.0f), width, height);   // for map size 15.0f
	Camera Camera(vec3(0.0f, 18.0f, 25.0f), vec3(0.0f, -0.9f, -1.0f), vec3(0.0f, 1.0f, 0.0f), (GLfloat)m_iWidth, (GLfloat)m_iHeight);
	//            Pos:  x      y      z    Front:       y tilt          Up:       

	AddCamera(Camera);
	CreateEntities();
	UpdateEntities();
	CreateTextFields();
	GetClock()->Initialise();

	// Sounds

	InitFmod();
	LoadAudio();
	m_pAudioMgr->playSound(m_pBGMusic, 0, false, &m_pBGChannel);
	m_pBGChannel->setVolume(0.00f);

	m_eNetworkEntityType = UNDEFINED;
}

/***********************
* GameScene create entities: Creates all of the entities needed for the game - player, map, enemies, bullets, power ups
* Parameters: none
* Return: void
********************/
void GameScene::CreateEntities() {

	// Player Cubes
	m_pPlayer1 = new Entity;
	m_pPlayer1->SetAsPlayer();
	m_pPlayer1->SetPlayerNumber(P1);
	m_pPlayer1->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(-15.0f, 0.0f, -15.0f), NONENEMY, 8.0f, "Assets//Textures//PlayerTexture.jpg");
	//m_pPlayer1->SetActivity(true);
	AddPlayer(m_pPlayer1);

	m_pPlayer2 = new Entity;
	m_pPlayer2->SetAsPlayer();
	m_pPlayer2->SetPlayerNumber(P2);
	m_pPlayer2->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(15.0f, 0.0f, -15.0f), NONENEMY, 8.0f, "Assets//Textures//PlayerTexture.jpg");
	//m_pPlayer2->SetActivity(true);
	AddPlayer(m_pPlayer2);

	m_pPlayer3 = new Entity;
	m_pPlayer3->SetAsPlayer();
	m_pPlayer3->SetPlayerNumber(P3);
	m_pPlayer3->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(-15.0f, 0.0f, 15.0f), NONENEMY, 8.0f, "Assets//Textures//PlayerTexture.jpg");
	//m_pPlayer3->SetActivity(true);
	AddPlayer(m_pPlayer3);

	m_pPlayer4 = new Entity;
	m_pPlayer4->SetAsPlayer();
	m_pPlayer4->SetPlayerNumber(P4);
	m_pPlayer4->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(15.0f, 0.0f, 15.0f), NONENEMY, 8.0f, "Assets//Textures//PlayerTexture.jpg");
	//m_pPlayer4->SetActivity(true);
	AddPlayer(m_pPlayer4);

	// Map Quad
	m_pMap = new Entity;
	m_pMap->Initialise(MAP, QUAD, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "Assets//Textures//Map.jpg");
	AddEntity(m_pMap);

	// Player 1 Bullets
	for (int x{ 0 }; x < 10; x++) {
		m_pBullet = new Entity;
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "Assets//Textures//PlayerBullet.png");
		m_pBullet->SetBulletPlayer(BP1);
		AddPlayer1Bullet(m_pBullet);
	}

	// Player 2 Bullets
	for (int x{ 0 }; x < 10; x++) {
		m_pBullet = new Entity;
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "Assets//Textures//PlayerBullet.png");
		m_pBullet->SetBulletPlayer(BP2);
		AddPlayer2Bullet(m_pBullet);
	}

	// Player 3 Bullets
	for (int x{ 0 }; x < 10; x++) {
		m_pBullet = new Entity;
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "Assets//Textures//PlayerBullet.png");
		m_pBullet->SetBulletPlayer(BP3);
		AddPlayer3Bullet(m_pBullet);
	}

	// Player 4 Bullets
	for (int x{ 0 }; x < 10; x++) {
		m_pBullet = new Entity;
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "Assets//Textures//PlayerBullet.png");
		m_pBullet->SetBulletPlayer(BP4);
		AddPlayer4Bullet(m_pBullet);
	}

	//// Player Bullets
	//for (int x{ 0 }; x < 10; x++) {
	//	m_pBullet = new Entity;
	//	m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "Assets//Textures//PlayerBullet.png");
	//	AddPlayerBullet(m_pBullet);
	//}

	// Enemy Bullets
	for (int y{ 0 }; y < 40; y++) {
		m_pBullet = new Entity;
		m_pBullet->SetAsEnemyBullet();
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "Assets//Textures//EnemyBullet.png");
		AddEnemyBullet(m_pBullet);
	}

	////////////////// Enemies //////////////////////////////////

	float fMaxVelocity = 0.05f;

	// 1
	m_pEnemy = new Entity;
	m_pEnemy->SetLeader(true);
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.1f, 0.0f, 0.1f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg"); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg"); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-2.0f, 0.0f, 2.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg"); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(2.0f, 0.0f, -2.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	// 6
	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-6.0f, 0.0f, -5.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 0.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-2.0f, 0.0f, -2.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	//m_pEnemy = new Entity;
	//m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(2.0f, 0.0f, 2.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	//AddEnemy(m_pEnemy);

	//m_pEnemy = new Entity;
	//m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	//AddEnemy(m_pEnemy);

	//// 11
	//m_pEnemy = new Entity;
	//m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 0.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	//AddEnemy(m_pEnemy);

	//m_pEnemy = new Entity;
	//m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 5.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	//AddEnemy(m_pEnemy);

	//m_pEnemy = new Entity;
	//m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, -5.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	//AddEnemy(m_pEnemy);

	//m_pEnemy = new Entity;
	//m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 0.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	//AddEnemy(m_pEnemy);

	//m_pEnemy = new Entity;
	//m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 0.0f), WANDER, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	//AddEnemy(m_pEnemy);

	// 15 enemies

	//// Extras for spawning in - 20 enemies 

	//for (int z{ 0 }; z < 20; z++) {
	//	m_pEnemy = new Entity;
	//	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.1f, 0.0f, 0.1f), SEEK, fMaxVelocity, "Assets//Textures//EnemyTexture.jpg");
	//	AddEnemy(m_pEnemy);
	//}

	// 35 Enemies total

	//////////////////////////////////////////////////////////////

	// Power Ups

	//m_pPowerUp = new Entity;
	//m_pPowerUp->Initialise(POWERUP, PYRAMID, 18, m_Camera, vec3(-2.5f, 0.0f, -2.0f), NONENEMY, 0.03f, "Assets//Textures//GreenPattern.png");
	//m_pPowerUp->SetPowerUpType(SPEEDBOOST);
	//AddPowerUp(m_pPowerUp);

	//m_pPowerUp = new Entity;
	//m_pPowerUp->Initialise(POWERUP, PYRAMID, 18, m_Camera, vec3(-2.5f, 0.0f, -2.0f), NONENEMY, 0.03f, "Assets//Textures//PinkPattern.png");
	//m_pPowerUp->SetPowerUpType(FASTFIRING);
	//AddPowerUp(m_pPowerUp);

	//m_pPowerUp = new Entity;
	//m_pPowerUp->Initialise(POWERUP, PYRAMID, 18, m_Camera, vec3(-2.5f, 0.0f, -2.0f), NONENEMY, 0.03f, "Assets//Textures//WhitePattern.png");
	//m_pPowerUp->SetPowerUpType(ENEMYFLEE);
	//AddPowerUp(m_pPowerUp);
}

/***********************
* GameScene create text fields: Creates all of the text needed for the game
* Parameters: none
* Return: void
********************/
void GameScene::CreateTextFields() {

	/// MENU
	{
		m_pTextLabel = new TextLabel(M_LABEL, "Robotron", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1.2f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight - 100));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(M_CURSOR, ">", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight - 300));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(M_LABEL, "PLAY", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 300));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(M_LABEL, "CREDITS", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 350));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(M_LABEL, "QUIT", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 400));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);
	}
	/// NETWORK_SELECT
	{
		m_pTextLabel = new TextLabel(NS_LABEL, "SELECT YOUR TYPE OF NETWORK", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 450, m_iHeight - 100));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(NS_CURSOR, ">", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight - 300));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(NS_LABEL, "SERVER", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 300));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(NS_LABEL, "CLIENT", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 350));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);
		m_pTextLabel = new TextLabel(NS_LABEL, "BACK TO MENU", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 400));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);
	}
	/// PLAY
	{
		m_pTextLabel = new TextLabel(P_LABEL, "Robotron", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(15, m_iHeight - 40));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LABEL, "Score:", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(15, m_iHeight - 80));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_AIDESCRIPTION, "", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(370, m_iHeight - 40));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_SCORE, "", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(250, m_iHeight - 80));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LABEL, "Lives:", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth - 300, m_iHeight - 80));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LABEL, "Level:", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth - 300, m_iHeight - 40));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LEVEL, "", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth - 70, m_iHeight - 40));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LIVES, "", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth - 70, m_iHeight - 80));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);
	}
	/// GAME OVER
	{
		m_pTextLabel = new TextLabel(G_LABEL, "GAME OVER", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1.3f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 230, m_iHeight / 2 + 200));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(G_LABEL, "Press Enter to return to the menu", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(0.7f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 435, m_iHeight / 2 - 300));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		// Score

		m_pTextLabel = new TextLabel(G_LABEL, "Score:", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1.1f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight / 2 + 10));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(G_SCORE, "", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1.1f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 + 70, m_iHeight / 2 + 10));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		// Level

		m_pTextLabel = new TextLabel(G_LABEL, "Level:", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1.1f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight / 2 - 100));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(G_LEVEL, "", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1.1f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 + 70, m_iHeight / 2 - 100));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);
	}

	/// CREDITS
	{
		m_pTextLabel = new TextLabel(C_LABEL, "Made by:", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 130, m_iHeight / 2 + 200));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(C_LABEL, "Mitchell Currie", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1.2f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 310, m_iHeight / 2 + 20));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(C_LABEL, "Juan Rodriguez", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(1.2f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 330, m_iHeight / 2 - 80));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(C_LABEL, "Press Enter to return to the menu", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setScale(0.7f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 435, m_iHeight / 2 - 300));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);
	}

}
#pragma endregion CREATE

#pragma region SETTERS
//void GameScene::SetPlayerAlive() {
//	m_pPlayer->SetToAlive();
//}

void GameScene::SetLevel(int _iLevel) {
	m_iCurrentLevel = _iLevel;
}

void GameScene::SetLevelComplete(bool _IsComplete) {
	m_bLevelComplete = _IsComplete;
}

void GameScene::SetAllBulletsInactive() {
	//for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++) {
	//	(*it)->SetActivity(false);
	//	//(*it)->ResetVelocityForBullets();		
	//}
	for (auto it = m_player1Bullets.begin(); it != m_player1Bullets.end(); it++) {
		(*it)->SetActive(false);
		//(*it)->ResetVelocityForBullets();		
	}

	for (auto it = m_player2Bullets.begin(); it != m_player2Bullets.end(); it++) {
		(*it)->SetActive(false);
		//(*it)->ResetVelocityForBullets();		
	}

	for (auto it = m_player3Bullets.begin(); it != m_player3Bullets.end(); it++) {
		(*it)->SetActive(false);
		//(*it)->ResetVelocityForBullets();		
	}

	for (auto it = m_player4Bullets.begin(); it != m_player4Bullets.end(); it++) {
		(*it)->SetActive(false);
		//(*it)->ResetVelocityForBullets();		
	}

	for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
		(*it)->SetActive(false);
		//(*it)->ResetVelocityForBullets();		
	}
}

void GameScene::AddCamera(Camera _camera) {
	m_Camera = _camera;
}

void GameScene::AddEntity(Entity* _entity) {
	m_entities.push_back(_entity);
}

void GameScene::AddPlayer(Entity* _entity) {
	m_players.push_back(_entity);
}

void GameScene::AddEnemy(Entity* _entity) {
	m_enemies.push_back(_entity);
}

void GameScene::AddPlayer1Bullet(Entity* _entity) {
	m_player1Bullets.push_back(_entity);
}

void GameScene::AddPlayer2Bullet(Entity* _entity) {
	m_player2Bullets.push_back(_entity);
}

void GameScene::AddPlayer3Bullet(Entity* _entity) {
	m_player3Bullets.push_back(_entity);
}

void GameScene::AddPlayer4Bullet(Entity* _entity) {
	m_player4Bullets.push_back(_entity);
}

void GameScene::AddEnemyBullet(Entity* _entity) {
	m_enemyBullets.push_back(_entity);
}

void GameScene::AddPowerUp(Entity* _entity) {
	m_powerUps.push_back(_entity);
}

void GameScene::AddText(TextLabel* _text) {
	m_textLabels.push_back(_text);
}
#pragma endregion SETTERS

#pragma region GETTERS
CClock* GameScene::GetClock() {
	return m_pClock;
}

Entity* GameScene::GetPlayer1() {
	return m_pPlayer1;
}

Entity* GameScene::GetPlayer2() {
	return m_pPlayer2;
}

Entity* GameScene::GetPlayer3() {
	return m_pPlayer3;
}

Entity* GameScene::GetPlayer4() {
	return m_pPlayer4;
}

bool GameScene::arePlayersActive() const {

	//if (!m_pPlayer1->IsActive() || !m_pPlayer2->IsActive() || !m_pPlayer3->IsActive() || !m_pPlayer4->IsActive())
	if (!m_pPlayer1->IsActive() || !m_pPlayer2->IsActive())
		return false;

	return true;

}

int GameScene::GetPlayerLives() {
	return m_iPlayerLives;
}

std::string GameScene::GetLives() {
	if (m_iPlayerLives < 0)
		return ("0");

	else
		return std::to_string(m_iPlayerLives);
}

void GameScene::ReducePlayerLives() {
	m_iPlayerLives--;
}

void GameScene::ResetPlayerLives() {
	m_iPlayerLives = 3;
}

bool GameScene::IsGameOver() {
	if (GetPlayerLives() < 0)
		return true;

	else
		return false;
}
#pragma endregion GETTERS

#pragma region ENTITY_CHECK
/***********************
* GameScene set positions: Calls the entity set positions function on each of the vectors of entities
* Parameters: float delta tick
* Return: void
********************/
void GameScene::SetPositions(float _fDeltaTick) {

	// Players
	for (auto it = m_players.begin(); it != m_players.end(); it++) {
		if ((*it)->IsActive() && (!(*it)->IsPlayerDead()))
			(*it)->SetPositions(_fDeltaTick);
	}

	// Map
	for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
		(*it)->SetPositions(_fDeltaTick);
	}

	// Enemies
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}

	//Player Bullets
	for (auto it = m_player1Bullets.begin(); it != m_player1Bullets.end(); it++) {
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}

	for (auto it = m_player2Bullets.begin(); it != m_player2Bullets.end(); it++) {
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}

	for (auto it = m_player3Bullets.begin(); it != m_player3Bullets.end(); it++) {
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}

	for (auto it = m_player4Bullets.begin(); it != m_player4Bullets.end(); it++) {
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}

	//Enemy Bullets
	for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}

	//Power ups
	for (auto it = m_powerUps.begin(); it != m_powerUps.end(); it++) {
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}
}

/***********************
* GameScene update entities: Resets the game entities and updates them for a new level
* Parameters: none
* Return: void
********************/
void GameScene::UpdateEntities() {

	// Players back to starting position
	for (auto it = m_players.begin(); it != m_players.end(); it++) {
		(*it)->GetModel()->ResetToStartingPosition();

		if ((*it)->IsActive())
			(*it)->SetToAlive();
	}

	// These commented out to allow powerups to carry over between levels
	// Uncomment to stop the effects when the level ends
	/*m_pPlayer->ResetMaxVelocity();
	m_pPlayer->ResetFireRate();
	m_pPlayer->SetSpedUp(false);
	m_pPlayer->SetFastFire(false);*/

	// Reset AI positions and set to inactive.
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		(*it)->GetModel()->ResetToStartingPosition();
		(*it)->GetModel()->ResetToBeDeleted();
		(*it)->SetActive(false);
		(*it)->SetLeader(false);
		(*it)->SetModelOutsideMap(false);
		(*it)->SetMaxVelocity(0.05f);
	}

	// Reset AI behaviours - from fleeing powerup
	ResetEnemyBehaviours();

	// Reset powerups and set to inactive.
	for (auto it = m_powerUps.begin(); it != m_powerUps.end(); it++) {
		(*it)->GetModel()->ResetToBeDeleted();
		(*it)->SetActive(false);
	}

	//Reset enemy counter
	Entity::ResetEnemyCounter();

	//Reset Leader back
	m_enemies.at(0)->SetLeader(true);

	m_iExtraEnemies = m_iCurrentLevel / 3;

	if (m_iCurrentLevel == 1) {
		for (int x{ 0 }; x < 7; x++) // Number of enemies
		{
			m_enemies.at(x)->SetAIBehaviour(FLEE);
			m_enemies.at(x)->SetActive(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 2) {
		for (int x{ 0 }; x < 7; x++) {
			m_enemies.at(x)->SetAIBehaviour(EVADE);
			m_enemies.at(x)->SetActive(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 3) {
		for (int x{ 0 }; x < 8; x++) {
			m_enemies.at(x)->SetAIBehaviour(WANDER);
			m_enemies.at(x)->SetActive(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 4) {
		for (int x{ 0 }; x < 6; x++) {
			m_enemies.at(x)->SetAIBehaviour(SEEK);
			m_enemies.at(x)->SetActive(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 5) {
		for (int x{ 0 }; x < 8; x++) {
			m_enemies.at(x)->SetAIBehaviour(LEADERFOLLOW);
			m_enemies.at(x)->SetActive(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 6) {
		for (int x{ 0 }; x < 8; x++) {
			m_enemies.at(x)->SetAIBehaviour(FLOCK);
			m_enemies.at(x)->SetActive(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else //if (m_iCurrentLevel == 7) {
		for (int x{ 0 }; x < 8; x++) {
			m_enemies.at(x)->SetAIBehaviour(PURSUE);
			m_enemies.at(x)->SetActive(true);
			Entity::IncrementEnemyCounter();
		}
	}

	//else // After level 7
	//{
	//	float fMaxVelocity = 0.05f + ((static_cast<float>(m_iCurrentLevel)) / 375.0f);
	//	//	float fMaxVelocity = 0.0001f;  // To test positions
	//	//	float fMaxVelocity = 0.05f;

	//	for (int x{ 0 }; x < 2; x++) {
	//		m_enemies.at(x)->SetAIBehaviour(SEEK);
	//		m_enemies.at(x)->SetActive(true);
	//		m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
	//		Entity::IncrementEnemyCounter();
	//	}

	//	for (int x{ 2 }; x < 4; x++) {
	//		m_enemies.at(x)->SetAIBehaviour(PURSUE);
	//		m_enemies.at(x)->SetActive(true);
	//		m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
	//		Entity::IncrementEnemyCounter();
	//	}

	//	for (int x{ 4 }; x < 11; x++) {
	//		m_enemies.at(x)->SetAIBehaviour(WANDER);
	//		m_enemies.at(x)->SetActive(true);
	//		m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
	//		Entity::IncrementEnemyCounter();
	//	}

	//	for (int x{ 11 }; x < 15; x++) {
	//		m_enemies.at(x)->SetAIBehaviour(FLOCK);
	//		m_enemies.at(x)->SetActive(true);
	//		m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
	//		Entity::IncrementEnemyCounter();
	//	}
	//}

/***********************
* GameScene check bullets: Checks if any bullets need to be activated or deactivated
* Parameters: none
* Return: void
********************/
void GameScene::CheckBullets() {

	//////// Player Bullets  //////////////////
	// Bullet fired

	// Players back to starting position
	if ((m_pPlayer1->IsActive())) {

		if (Entity::IsP1BulletFired()) {

			m_player1Bullets.at(Entity::GetPlayer1BulletCounter())->SetActive(true); // set next bullet in vector to active
			m_player1Bullets.at(Entity::GetPlayer1BulletCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
			m_player1Bullets.at(Entity::GetPlayer1BulletCounter())->SetModelPosition(m_pPlayer1->GetModel()->GetPosition()); // Set position to players current position

			Entity::IncrementPlayer1BulletCounter(); // increment the bullet counter for the next bullet fired
			m_pAudioMgr->playSound(m_pShootSound, 0, false, &m_pShootChannel);

		}

		// Check if any bullets need to be deleted - either through collision with object or edge of map
		for (auto it = m_player1Bullets.begin(); it != m_player1Bullets.end(); it++) {

			if ((*it)->ToDelete()) {

				(*it)->SetActive(false); // set to inactive
				(*it)->ResetVelocityForPlayerBullets(); // reset velocity

			}

		}

	}

	if ((m_pPlayer2->IsActive()))// && !(m_pPlayer2->IsPlayerDead()))
	{
		if (Entity::IsP2BulletFired()) {
			m_player2Bullets.at(Entity::GetPlayer2BulletCounter())->SetActive(true); // set next bullet in vector to active
			m_player2Bullets.at(Entity::GetPlayer2BulletCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
			m_player2Bullets.at(Entity::GetPlayer2BulletCounter())->SetModelPosition(m_pPlayer2->GetModel()->GetPosition()); // Set position to players current position

			Entity::IncrementPlayer2BulletCounter(); // increment the bullet counter for the next bullet fired
			m_pAudioMgr->playSound(m_pShootSound, 0, false, &m_pShootChannel);
		}

		// Check if any bullets need to be deleted - either through collision with object or edge of map
		for (auto it = m_player2Bullets.begin(); it != m_player2Bullets.end(); it++) {
			if ((*it)->ToDelete()) {
				(*it)->SetActive(false); // set to inactive
				(*it)->ResetVelocityForPlayerBullets(); // reset velocity
			}
		}
	}

	if ((m_pPlayer3->IsActive()))// && !(m_pPlayer3->IsPlayerDead()))
	{
		if (Entity::IsP3BulletFired()) {
			m_player3Bullets.at(Entity::GetPlayer3BulletCounter())->SetActive(true); // set next bullet in vector to active
			m_player3Bullets.at(Entity::GetPlayer3BulletCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
			m_player3Bullets.at(Entity::GetPlayer3BulletCounter())->SetModelPosition(m_pPlayer3->GetModel()->GetPosition()); // Set position to players current position

			Entity::IncrementPlayer3BulletCounter(); // increment the bullet counter for the next bullet fired
			m_pAudioMgr->playSound(m_pShootSound, 0, false, &m_pShootChannel);
		}

		// Check if any bullets need to be deleted - either through collision with object or edge of map
		for (auto it = m_player3Bullets.begin(); it != m_player3Bullets.end(); it++) {
			if ((*it)->ToDelete()) {
				(*it)->SetActive(false); // set to inactive
				(*it)->ResetVelocityForPlayerBullets(); // reset velocity
			}
		}
	}

	if ((m_pPlayer4->IsActive()))// && !(m_pPlayer4->IsPlayerDead()))
	{
		if (Entity::IsP4BulletFired()) {
			m_player4Bullets.at(Entity::GetPlayer4BulletCounter())->SetActive(true); // set next bullet in vector to active
			m_player4Bullets.at(Entity::GetPlayer4BulletCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
			m_player4Bullets.at(Entity::GetPlayer4BulletCounter())->SetModelPosition(m_pPlayer4->GetModel()->GetPosition()); // Set position to players current position

			Entity::IncrementPlayer4BulletCounter(); // increment the bullet counter for the next bullet fired
			m_pAudioMgr->playSound(m_pShootSound, 0, false, &m_pShootChannel);
		}

		// Check if any bullets need to be deleted - either through collision with object or edge of map
		for (auto it = m_player4Bullets.begin(); it != m_player4Bullets.end(); it++) {
			if ((*it)->ToDelete()) {
				(*it)->SetActive(false); // set to inactive
				(*it)->ResetVelocityForPlayerBullets(); // reset velocity
			}
		}
	}

	//////// Enemy Bullets  //////////////////

	// Randomise whether each enemy should fire

	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		if ((rand() % 130 == 1) && (*it)->IsActive() && (!(*it)->GetModel()->IsOutsideMap())) // 1/130 chance of firing
		{
			m_enemyBullets.at(Entity::GetEnemyBulletCounter())->SetActive(true); // set next bullet in vector to active
			m_enemyBullets.at(Entity::GetEnemyBulletCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
			m_enemyBullets.at(Entity::GetEnemyBulletCounter())->SetModelPosition((*it)->GetModel()->GetPosition()); // Set position to enemies current position
			Entity::IncrementEnemyBulletCounter();
		}
	}

	// Check if any bullets need to be deleted - either through collision with object or edge of map 
	for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
		if ((*it)->ToDelete()) {
			(*it)->SetActive(false); // set to inactive
			(*it)->VelocityToZero(); // reset velocity
		}
	}
}

/***********************
* GameScene check enemies: Checks if any enemies need to be activated or deactivated
* Parameters: none
* Return: void
********************/
void GameScene::CheckEnemies() {
	// Extra enemy spawning
	if ((m_iCurrentLevel > 9) && (m_iExtraEnemies > 0)) {
		m_durationAITimer = (std::clock() - m_startAITimer) / (double)CLOCKS_PER_SEC;
		float fMaxVelocity = 0.05f + ((static_cast<float>(m_iCurrentLevel)) / 200.0f);
		float fTimer = 0.0f;
		float iRandom1 = 0.0f;
		float iRandom2 = 0.0f;

		if (m_iCurrentLevel < 30)
			fTimer = -0.1f * m_iCurrentLevel + 3.5f;
		else
			fTimer = 0.5f;

		if (m_durationAITimer > fTimer) {
			while ((iRandom1 == 0.0f) && (iRandom2 == 0.0f)) // Get random numbers either -30, 30 or 0, but don't want 0,0 because they will spawn in the middle of the map
			{
				iRandom1 = GetRandomPosition();
				iRandom2 = GetRandomPosition();
			}

			m_enemies.at(Entity::GetEnemyCounter())->SetActive(true); // set next bullet in vector to active
			m_enemies.at(Entity::GetEnemyCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
			m_enemies.at(Entity::GetEnemyCounter())->SetModelPosition(vec3(iRandom1, 0.0f, iRandom2)); // Set position
			m_enemies.at(Entity::GetEnemyCounter())->SetMaxVelocity(fMaxVelocity);
			m_enemies.at(Entity::GetEnemyCounter())->SetModelOutsideMap(true);
			Entity::IncrementEnemyCounter();

			m_startAITimer = std::clock();
			m_iExtraEnemies--;
		}
	}


	// Check if any enemies need to be deleted - either through collision with bullet or player
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		if ((*it)->ToDelete()) {
			(*it)->SetActive(false); // set to inactive
		}
	}
}

/***********************
* GameScene check power ups: Checks if any power ups need to be activated or deactivated
* Parameters: none
* Return: void
********************/
void GameScene::CheckPowerUps() {
	int i = rand() % 3;

	int iRand = 0;

	if (m_iCurrentLevel <= 10)
		iRand = 300;

	else if ((m_iCurrentLevel > 10) && (m_iCurrentLevel > 20))
		iRand = 250;

	else // Level 20 and above
		iRand = 200;

	// Creating power ups
	if ((rand() % iRand == 39) && !AllPowerUpsActive())   // change back to 1 in 400
	{
		while (m_powerUps.at(i)->IsActive()) {
			i = rand() % 3;
		}

		m_powerUps.at(i)->SetActive(true); // set random power up to active
		m_powerUps.at(i)->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
		m_powerUps.at(i)->SetModelPosition(GetRandomMapPosition()); // Set position		
	}

	// Check if any powerups need to be deleted - either through collision with bullet or player
	for (auto it = m_powerUps.begin(); it != m_powerUps.end(); it++) {
		if ((*it)->ToDelete()) {
			(*it)->SetActive(false); // set to inactive
		}
	}

	// Speed boosts
	for (auto it = m_players.begin(); it != m_players.end(); it++) {
		if ((*it)->IsActive()) {
			m_durationSpeedBoostTimer = (std::clock() - m_startSpeedBoostTimer) / (double)CLOCKS_PER_SEC;

			if (m_durationSpeedBoostTimer > 5.0f) {
				(*it)->ResetMaxVelocity();
				(*it)->SetSpedUp(false);
				m_startSpeedBoostTimer = std::clock();
			}
		}
	}

	// Fast firing
	for (auto it = m_players.begin(); it != m_players.end(); it++) {
		if ((*it)->IsActive()) {
			m_durationFastFireTimer = (std::clock() - m_startFastFireTimer) / (double)CLOCKS_PER_SEC;

			if (m_durationFastFireTimer > 5.0f) {
				(*it)->ResetFireRate();
				(*it)->SetFastFire(false);
				m_startFastFireTimer = std::clock();
			}
		}
	}

	// Enemy fleeing

	m_durationEnemyFleeTimer = (std::clock() - m_startEnemyFleeTimer) / (double)CLOCKS_PER_SEC;

	if (m_durationEnemyFleeTimer > 2.0f) {
		ResetEnemyBehaviours();
		m_startEnemyFleeTimer = std::clock();
	}

	//Extra Lives

	//Extra Lives

	//if (m_pPlayer->GetScoreCounter() >= 1000) {
	//	m_pPlayer->AddExtraLife();
	//	m_pPlayer->ResetScoreCounter();
	//}

	if (GetScoreCounter() >= 1000) {
		AddExtraLife();
		ResetScoreCounter();
	}
}
#pragma endregion ENTITY_CHECK

#pragma region COLLISION
/***********************
* GameScene check collisions: Checks all the collisions in the game between the different entity types
* Parameters: none
* Return: void
********************/
void GameScene::CheckCollisions() {
	// Enemy and player collisions
	for (auto it = m_players.begin(); it != m_players.end(); it++) {
		for (auto it2 = m_enemies.begin(); it2 != m_enemies.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.2f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.2f) &&
				((*it)->IsActive()) && (*it2)->IsActive()) {
				(*it)->SetToDead();
				//		m_pAudioMgr->playSound(m_pDieSound, 0, false, &m_pDieChannel);
				return;
			}
		}
	}

	// Enemy bullet and player collisions
	for (auto it = m_players.begin(); it != m_players.end(); it++) {
		for (auto it2 = m_enemyBullets.begin(); it2 != m_enemyBullets.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
				((*it)->IsActive()) && ((*it2)->IsActive())) {
				(*it)->SetToDead();
				//		m_pAudioMgr->playSound(m_pDieSound, 0, false, &m_pDieChannel);
				return;

				//break;
			}
		}
	}

	// Player bullet and enemy collisions
	// Player 1
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		for (auto it2 = m_player1Bullets.begin(); it2 != m_player1Bullets.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
				((*it)->GetEntityType() == ENEMY) && ((*it2)->IsActive()) && ((*it)->IsActive())) {
				(*it)->GetModel()->SetToBeDeleted(); // set to delete enemy
				(*it2)->GetModel()->SetToBeDeleted(); // set to delete player bullet

				if ((*it)->GetModel()->IsLeader()) {
					(*it)->SetLeaderDead();
					(*it)->SetLeader(false);
					(*it)->GetModel()->SetLeader(false);

				}

				//			m_pPlayer->AddToScore(10);
				AddToScore(10);
				m_pAudioMgr->playSound(m_pHitSound, 0, false, &m_pHitChannel);

				break;
			}
		}
	}

	// Player 2
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		for (auto it2 = m_player2Bullets.begin(); it2 != m_player2Bullets.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
				((*it)->GetEntityType() == ENEMY) && ((*it2)->IsActive()) && ((*it)->IsActive())) {
				(*it)->GetModel()->SetToBeDeleted(); // set to delete enemy
				(*it2)->GetModel()->SetToBeDeleted(); // set to delete player bullet

				if ((*it)->GetModel()->IsLeader()) {
					(*it)->SetLeaderDead();
					(*it)->SetLeader(false);
					(*it)->GetModel()->SetLeader(false);

				}

				//			m_pPlayer->AddToScore(10);
				AddToScore(10);
				m_pAudioMgr->playSound(m_pHitSound, 0, false, &m_pHitChannel);

				break;
			}
		}
	}

	// Player 3
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		for (auto it2 = m_player3Bullets.begin(); it2 != m_player3Bullets.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
				((*it)->GetEntityType() == ENEMY) && ((*it2)->IsActive()) && ((*it)->IsActive())) {
				(*it)->GetModel()->SetToBeDeleted(); // set to delete enemy
				(*it2)->GetModel()->SetToBeDeleted(); // set to delete player bullet

				if ((*it)->GetModel()->IsLeader()) {
					(*it)->SetLeaderDead();
					(*it)->SetLeader(false);
					(*it)->GetModel()->SetLeader(false);

				}

				//			m_pPlayer->AddToScore(10);
				AddToScore(10);
				m_pAudioMgr->playSound(m_pHitSound, 0, false, &m_pHitChannel);

				break;
			}
		}
	}

	// Player 4
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		for (auto it2 = m_player4Bullets.begin(); it2 != m_player4Bullets.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
				((*it)->GetEntityType() == ENEMY) && ((*it2)->IsActive()) && ((*it)->IsActive())) {
				(*it)->GetModel()->SetToBeDeleted(); // set to delete enemy
				(*it2)->GetModel()->SetToBeDeleted(); // set to delete player bullet

				if ((*it)->GetModel()->IsLeader()) {
					(*it)->SetLeaderDead();
					(*it)->SetLeader(false);
					(*it)->GetModel()->SetLeader(false);

				}

				//			m_pPlayer->AddToScore(10);
				AddToScore(10);
				m_pAudioMgr->playSound(m_pHitSound, 0, false, &m_pHitChannel);

				break;
			}
		}
	}
	////////////////  To Add later /////////  Change to check all 4 players' bullets

	// Player bullet and enemy bullet collisions
	//for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
	//	for (auto it2 = m_playerBullets.begin(); it2 != m_playerBullets.end(); it2++) {
	//		if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 0.5f) &&   // within a distance
	//			(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 0.5f) &&
	//			((*it2)->IsActive()) && ((*it)->IsActive())) {
	//			(*it)->GetModel()->SetToBeDeleted(); // set to delete enemy bullet
	//			(*it2)->GetModel()->SetToBeDeleted(); // set to delete player bullet

	//												  //break;
	//		}
	//	}
	//}

	//// Player and power up collisions
	//for (auto it = m_players.begin(); it != m_players.end(); it++) {
	//	for (auto it2 = m_powerUps.begin(); it2 != m_powerUps.end(); it2++) {
	//		if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
	//			(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
	//			((*it)->IsActive()) && ((*it2)->IsActive())) {
	//			(*it2)->GetModel()->SetToBeDeleted();

	//			m_pPlayer->AddToScore(20);
	//			m_pAudioMgr->playSound(m_pPowerUpSound, 0, false, &m_pPowerUpChannel);
	//			m_pPowerUpChannel->setVolume(0.5);

	//			if ((*it2)->GetPowerUpType() == SPEEDBOOST) {
	//				(*it)->SpeedUp();
	//				(*it)->SetSpedUp(true);
	//				m_startSpeedBoostTimer = std::clock();
	//			}

	//			if ((*it2)->GetPowerUpType() == FASTFIRING) {
	//				(*it)->FastFire();
	//				(*it)->SetFastFire(true);
	//				m_startFastFireTimer = std::clock();
	//			}

	//			if ((*it2)->GetPowerUpType() == ENEMYFLEE) {
	//				SetEnemiesToFlee();
	//				m_startEnemyFleeTimer = std::clock();
	//			}

	//			return;

	//			//break;
	//		}
	//	}
	//}

	// Enemy and wall collisions
	float fMapSize = MAP_SIZE - 0.5f;

	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		if ((!((*it)->GetAIBehaviour() == LEADERFOLLOW)))// && (!((*it)->GetAIBehaviour() == FLOCK)))
		{
			if (((*it)->GetModel()->HasEnteredMap()) && ((*it)->GetModel()->IsOutsideMap()))
				(*it)->GetModel()->SetOutsideMap(false);

			else if (((*it)->GetModel()->HasExitedMap()) && (!((*it)->GetModel()->IsOutsideMap())))
				(*it)->GetModel()->SetOutsideMap(true);

			////Right side and bottom
			//if (((((*it)->GetModel()->GetPosition().x) > fMapSize) ||
			//	(((*it)->GetModel()->GetPosition().z) > fMapSize)) &&
			//	((*it)->IsActive()) && //(!((*it)->IsFleeing())) &&//((!((*it)->GetAIBehaviour() == WANDER)) || ((*it)->IsFleeing())) &&*/
			//	(!((*it)->GetModel()->IsOutsideMap())))
			//{
			//	(*it)->Seek(vec3(0.0f, 0.0f, 0.0f));
			//}

			//Right
			if ((((*it)->GetModel()->GetPosition().x) > fMapSize) &&
				((*it)->IsActive()) && //(!((*it)->IsFleeing())) &&//((!((*it)->GetAIBehaviour() == WANDER)) || ((*it)->IsFleeing())) &&*/
				(!((*it)->GetModel()->IsOutsideMap()))) {
				//	(*it)->Seek(vec3(1.0f, 0.0f, 1.0f));	
				//	(*it)->Seek(-((*it)->GetModel()->GetPosition()));
				//	((*it)->GetModel()->GetPosition().x)
				//	(*it)->Seek(vec3(fMapSize, 0.0f, (*it)->GetModel()->GetPosition().z));
				int iRand = rand() % 2;

				if (iRand == 0)
					(*it)->GetModel()->SetPosition(vec3(fMapSize, 0.0f, ((*it)->GetModel()->GetPosition().z)));
				else
					(*it)->Seek(vec3(0.0f, 0.0f, 0.0f));
			}

			//Left
			if ((((*it)->GetModel()->GetPosition().x) < -fMapSize) &&
				((*it)->IsActive()) && //(!((*it)->IsFleeing())) &&//((!((*it)->GetAIBehaviour() == WANDER)) || ((*it)->IsFleeing())) &&*/
				(!((*it)->GetModel()->IsOutsideMap()))) {
				//	(*it)->Seek(vec3(1.0f, 0.0f, 1.0f));	
				//	(*it)->Seek(-((*it)->GetModel()->GetPosition()));
				//	((*it)->GetModel()->GetPosition().x)
				//	(*it)->Seek(vec3(fMapSize, 0.0f, (*it)->GetModel()->GetPosition().z));
				(*it)->GetModel()->SetPosition(vec3(-fMapSize, 0.0f, ((*it)->GetModel()->GetPosition().z)));
			}


			//Top
			if ((((*it)->GetModel()->GetPosition().z) < -fMapSize) &&
				((*it)->IsActive()) && //(!((*it)->IsFleeing())) &&//((!((*it)->GetAIBehaviour() == WANDER)) || ((*it)->IsFleeing())) &&*/
				(!((*it)->GetModel()->IsOutsideMap()))) {
				//	(*it)->Seek(vec3(1.0f, 0.0f, 1.0f));
				//	(*it)->Seek(-((*it)->GetModel()->GetPosition()));
				//	((*it)->GetModel()->GetPosition().x)
				//	(*it)->Seek(vec3((*it)->GetModel()->GetPosition().z, 0.0f, fMapSize));
				(*it)->GetModel()->SetPosition(vec3(((*it)->GetModel()->GetPosition().x), 0.0f, -fMapSize));
			}

			//Bottom
			if ((((*it)->GetModel()->GetPosition().z) > fMapSize) &&
				((*it)->IsActive()) && //(!((*it)->IsFleeing())) &&//((!((*it)->GetAIBehaviour() == WANDER)) || ((*it)->IsFleeing())) &&*/
				(!((*it)->GetModel()->IsOutsideMap()))) {
				//	(*it)->Seek(vec3(1.0f, 0.0f, 1.0f));
				//	(*it)->Seek(-((*it)->GetModel()->GetPosition()));
				//	((*it)->GetModel()->GetPosition().x)
				//	(*it)->Seek(vec3((*it)->GetModel()->GetPosition().z, 0.0f, fMapSize));
				int iRand = rand() % 2;

				if (iRand == 0)
					(*it)->GetModel()->SetPosition(vec3(((*it)->GetModel()->GetPosition().x), 0.0f, fMapSize));

				else
					(*it)->Seek(vec3(0.0f, 0.0f, 0.0f));
			}
		}
	}

	// Enemy and enemy collisions
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		for (auto it2 = m_enemies.begin(); it2 != m_enemies.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.2f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.2f) &&   // within a distance
				(((*it)->GetModel()->GetPosition().x) != ((*it2)->GetModel()->GetPosition().x)) &&      // but not equal - that would be the same object
				((*it)->GetModel()->GetPosition().z) != ((*it2)->GetModel()->GetPosition().z) &&
				((*it)->IsActive()) && ((*it2)->IsActive())) {
				(*it)->Flee((*it2)->GetModel()->GetPosition());
				//		break;						
			}
		}
	}
}
#pragma endregion COLLISION

#pragma region LEVELS
bool GameScene::IsLevelComplete() {

	// Check if level complete - no active enemies
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		if ((*it)->IsActive()) {
			m_bLevelComplete = false;
			return m_bLevelComplete;
		}
	}

	m_bLevelComplete = true;
	return m_bLevelComplete;
}

void GameScene::NextLevel() {
	m_iCurrentLevel++;
}

bool GameScene::AllPlayersDead() {

	int playerInactive = 0;
	int playerDead = 0;

	for (auto it = m_players.begin(); it != m_players.end(); it++) {

		if (!(*it)->IsActive())
			playerInactive++;

	}

	if (m_players.size() == playerInactive)
		return false;

	for (auto it = m_players.begin(); it != m_players.end(); it++) {

		if ((*it)->IsActive() && (!(*it)->IsPlayerDead()))
			return false;

	}

	return true;
}

void GameScene::SetPlayersToAlive() {
	for (auto it = m_players.begin(); it != m_players.end(); it++) {

		if ((*it)->IsActive())
			(*it)->SetToAlive();
	}
}

void GameScene::AddExtraLife() {
	m_iPlayerLives++;
}
#pragma endregion LEVELS

#pragma region SCORE
void GameScene::AddToScore(int _Score) {
	m_iScore += _Score;
	m_iScoreCounter += _Score;
}

std::string GameScene::GetScore() {
	return std::to_string(m_iScore);
}

void GameScene::ResetScore() {
	m_iScore = 0;
	m_iScoreCounter = 0;
}

void GameScene::ResetScoreCounter() {
	m_iScoreCounter = 0;
}

int GameScene::GetScoreCounter() {
	return m_iScoreCounter;
}

#pragma endregion SCORE

#pragma region AI
void GameScene::SetEnemiesToFlee() {
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		(*it)->SetEnemyToFlee(true);
	}
}

void GameScene::ResetEnemyBehaviours() {
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		(*it)->SetEnemyToFlee(false);
	}
}

// AI
float GameScene::GetRandomPosition() {
	int iRandom = rand() % 3;

	if (iRandom == 0)
		return MAP_SIZE * 2.0f;

	else if (iRandom == 1)
		return MAP_SIZE * -2.0f;

	else
		return 0.0f;
}


#pragma endregion AI

#pragma region POWERUP
// Powerup
vec3 GameScene::GetRandomMapPosition() {
	vec3 temp = vec3(0.0f, 0.0f, 0.0f);

	int iMapSize = static_cast<int>(MAP_SIZE);

	temp.x = static_cast<float>((rand() % ((iMapSize * 2) - 1)) - (iMapSize - 1));
	temp.z = static_cast<float>((rand() % ((iMapSize * 2) - 1)) - (iMapSize - 1));


	return temp;
}

bool GameScene::AllPowerUpsActive() {
	for (auto it = m_powerUps.begin(); it != m_powerUps.end(); it++) {
		if (!(*it)->IsActive())
			return false;
	}

	return true;
}
#pragma endregion POWERUP

#pragma region UPDATE
/***********************
* GameScene update: Updates the game based on the current game state
* Parameters: none
* Return: void
********************/
void GameScene::Update() {

	GetClock()->Process();
	m_fDeltaTick = GetClock()->GetDeltaTick();

	/*
	------------ MENU
	*/
	if (m_iGameState == MENU) {

		if (keyState[(unsigned char)3] == BUTTON_DOWN || keyState[(unsigned char)13] == BUTTON_DOWN) {

			m_pAudioMgr->playSound(m_pSelectSound, 0, false, &m_pSelectChannel);

			if (m_iCursorIndex == 0) {

				ResetPlayerLives();
				ResetScore();
				m_iCurrentLevel = 1;
				UpdateEntities();
				m_iGameState = NETWORK_SELECT;
				m_iCursorIndex = 0;

			}
			else if (m_iCursorIndex == 1) {

				m_iGameState = CREDITS;

			}
			else if (m_iCursorIndex == 2) {

				glutDestroyWindow(1);

			}

			Sleep(130);

		}
		else if ((keyState[(unsigned char)'w'] == BUTTON_DOWN) || (keyState[(unsigned char)'W'] == BUTTON_DOWN)) {

			m_iCursorIndex--;
			m_pAudioMgr->playSound(m_pScrollSound, 0, false, &m_pScrollChannel);
			Sleep(130);

		}
		else if ((keyState[(unsigned char)'s'] == BUTTON_DOWN) || (keyState[(unsigned char)'S'] == BUTTON_DOWN)) {

			m_iCursorIndex++;
			m_pAudioMgr->playSound(m_pScrollSound, 0, false, &m_pScrollChannel);
			Sleep(130);

		}

		if (m_iCursorIndex < 0) {

			m_iCursorIndex = MENU_CHOICES - 1;

		}
		else if (m_iCursorIndex >= MENU_CHOICES) {

			m_iCursorIndex = 0;

		}

	}
	/*
	------------ NETWORK SELECTION
	*/
	else if (m_iGameState == NETWORK_SELECT) {

		if (keyState[(unsigned char)3] == BUTTON_DOWN || keyState[(unsigned char)13] == BUTTON_DOWN) {

			m_pAudioMgr->playSound(m_pSelectSound, 0, false, &m_pSelectChannel);

			if (m_eNetworkEntityType == UNDEFINED) {

				if (m_iCursorIndex == 0) {

					m_eNetworkEntityType = SERVER;
					CreateNetwork();
					m_pServer->CreateEntities();

				}
				else if (m_iCursorIndex == 1) {

					m_eNetworkEntityType = CLIENT;
					CreateNetwork();
					m_iCursorIndex = 0;

				}
				else if (m_iCursorIndex == 2) {

					m_iGameState = MENU;
					m_iCursorIndex = 0;

				}

			}
			// CHECK FOR CLIENT CONNECTION
			else if (m_eNetworkEntityType == CLIENT) {

				m_pClient->ConnectToServer(m_iCursorIndex);

			}

			Sleep(130);

		}
		else if ((keyState[(unsigned char)'w'] == BUTTON_DOWN) || (keyState[(unsigned char)'W'] == BUTTON_DOWN)) {

			m_iCursorIndex--;
			m_pAudioMgr->playSound(m_pScrollSound, 0, false, &m_pScrollChannel);
			Sleep(130);

		}
		else if ((keyState[(unsigned char)'s'] == BUTTON_DOWN) || (keyState[(unsigned char)'S'] == BUTTON_DOWN)) {

			m_iCursorIndex++;
			m_pAudioMgr->playSound(m_pScrollSound, 0, false, &m_pScrollChannel);
			Sleep(130);

		}

		if (m_eNetworkEntityType == UNDEFINED) {

			if (m_iCursorIndex < 0) {

				m_iCursorIndex = MENU_CHOICES - 1;

			}
			else if (m_iCursorIndex >= MENU_CHOICES) {

				m_iCursorIndex = 0;

			}

		}
		else if (m_eNetworkEntityType == SERVER) {

			UpdateServer();

		}
		else if (m_eNetworkEntityType == CLIENT) {

			if (m_iCursorIndex < 0) {

				m_iCursorIndex = m_iClientChoices - 1;

			}
			else if (m_iCursorIndex >= m_iClientChoices) {

				m_iCursorIndex = 0;

			}

			// Player joins the game
			std::string name = m_pClient->GetName();
			if (name != "") {

				username = _strdup(name.c_str());
				usernameC = _strdup(username.c_str());

				if (name == " P1") {
					m_players[0]->SetActive(true);
				}
				else if (name == " P2") {
					m_players[1]->SetActive(true);

				}
				else if (name == " P3") {
					m_players[2]->SetActive(true);

				}
				else if (name == " P4") {
					m_players[3]->SetActive(true);

				}

				m_packet.Serialize(COMMAND, usernameC, "!A");
				m_pClient->SendData(m_packet.PacketData);

				m_iGameState = PLAY;

			}

		}

	}
	/*
	------------ PLAY
	*/
	else if (m_iGameState == PLAY) {

		if (keyState[(unsigned char)27] == BUTTON_DOWN) {

			m_packet.Serialize(QUIT, m_pClient->GetName(), "SERVER");
			m_pClient->SendData(m_packet.PacketData);
			Sleep(150);
			m_iGameState = MENU;

		}
		else if (m_eNetworkEntityType == CLIENT) {
			UpdateClient();
			UpdateEnemiesForClient();
		}

	}
	/*
	------------ GAME OVER
	*/
	else if (m_iGameState == GAME_OVER) {

		if ((keyState[(unsigned char)3] == BUTTON_DOWN || keyState[(unsigned char)13] == BUTTON_DOWN)) {
			m_pAudioMgr->playSound(m_pSelectSound, 0, false, &m_pSelectChannel);
			m_iGameState = MENU;
			SetLevel(1);
			Sleep(200);
		}

	}
	/*
	------------ CREDITS
	*/
	else if (m_iGameState == CREDITS) {

		if ((keyState[(unsigned char)3] == BUTTON_DOWN || keyState[(unsigned char)13] == BUTTON_DOWN)) {
			m_pAudioMgr->playSound(m_pSelectSound, 0, false, &m_pSelectChannel);
			m_iGameState = MENU;
			Sleep(200);
		}
	}

}

void GameScene::UpdateServer() {

	// Check all the game info based on positions stored in server:

//	if ((m_eNetworkEntityType == SERVER) && (m_pServer->HasGameStarted())) {

	if (!m_pServer->HasGameStarted())
		return;

	//if (AllPlayersDead()) {
	//	SetAllBulletsInactive();
	//	ReducePlayerLives();

	//	if (IsGameOver()) {
	//		//SetLevel(1);  // go back to level one
	//		//GetPlayer()->ResetPlayerLives();
	//		ResetPlayerLives();
	//		//GetPlayer()->ResetScore();
	//		m_iGameState = GAME_OVER;

	//	}

	UpdateEntities(); // restart level		
	//}

	//else if (IsLevelComplete() && (m_iCurrentLevel > 0)) {
	//	SetAllBulletsInactive(); // To stop bullet rendering
	//	NextLevel();
	//	UpdateEntities();   // start the next level
	//	SetLevelComplete(false);
	//}

	CheckEnemies();  // Check if any enemies need to be deactivated - if set "to delete"
	//CheckBullets();  // Check if any bullets need to be deactivated - if set "to delete"
	//CheckPowerUps();

	//CheckCollisions();



	//Server vector of enemies needs to be updated from the gamscene vector of enemies
	std::vector<Enemy*> tempEnemies;

	//for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
	for (int y{ 0 }; y < 8; y++){
	
		Enemy temp;
		temp.SetPosition(m_enemies[y]->GetModel()->GetPosition());
		temp.SetActivity(m_enemies[y]->IsActive());
		tempEnemies.push_back(&temp);

	}

	m_pServer->SetEnemies(tempEnemies);

	// Send info to players

	//// Player positions
	//TPacket _packetToSend;

	//_packetToSend.SerializePosition(POSITION, " P1", "-", m_pServer->GetPlayer("P1")->GetPosition());
	//m_pServer->SendDataToAll(_packetToSend.PacketData);

	//_packetToSend.SerializePosition(POSITION, " P2", "-", m_pServer->GetPlayer("P2")->GetPosition());
	//m_pServer->SendDataToAll(_packetToSend.PacketData);

	//_packetToSend.SerializePosition(POSITION, " P3", "-", m_pServer->GetPlayer("P3")->GetPosition());
	//m_pServer->SendDataToAll(_packetToSend.PacketData);

	//_packetToSend.SerializePosition(POSITION, " P4", "-", m_pServer->GetPlayer("P4")->GetPosition());
	//m_pServer->SendDataToAll(_packetToSend.PacketData);

//	}

//	m_pServer->SendPositionsToPlayers();

// Player and player collisions
	//std::vector<Player*> mp = m_pServer->GetPlayers();

	//for (auto it = mp.begin(); it != mp.end(); it++) {

	//	/*if (!(*it)->IsActive())
	//		continue;*/

	//	for (auto it2 = mp.begin(); it2 < mp.end(); it2++) {

	//		/*if (!(*it2)->IsActive())
	//			continue;*/

	//		if ((abs(((*it)->GetPosition().x) - ((*it2)->GetPosition().x)) < 1.2f) &&   // within a distance
	//			(abs(((*it)->GetPosition().z) - ((*it2)->GetPosition().z)) < 1.2f) &&   // within a distance
	//			(((*it)->GetPosition().x) != ((*it2)->GetPosition().x)) &&      // but not equal - that would be the same object
	//			((*it)->GetPosition().z) != ((*it2)->GetPosition().z)
	//		
	//		&& ((*it)->IsActive()) && ((*it2)->IsActive())) {

	//			//std::cout << "Collision!" << std::endl;
	//			m_packet.Serialize(COMMAND, (*it)->GetName(), "!C");
	//			m_pServer->SendDataToAll(m_packet.PacketData);

	//		}

	//		/*else {
	//			m_packet.Serialize(COMMAND, (*it)->GetName(), "!N");
	//			m_pServer->SendDataToAll(m_packet.PacketData);
	//		}*/
	//	}
	//}


}

void GameScene::UpdateClient() {

	if (!m_pClient->HasGameStarted())
		return;

	// Checking which players are active
	if (m_pClient->IsActive("P1")) {
		m_pPlayer1->SetActive(true);
	}
	if (m_pClient->IsActive("P2")) {
		m_pPlayer2->SetActive(true);
	}
	if (m_pClient->IsActive("P3")) {
		m_pPlayer3->SetActive(true);
	}
	if (m_pClient->IsActive("P4")) {
		m_pPlayer4->SetActive(true);
	}

	// Setting the clients position and sending to the server

	if (username == " P1") {

	//	if (!m_pClient->HasCollided())
			m_pPlayer1->SetPositions(m_fDeltaTick);

		if (m_pPlayer2->IsActive())
			m_pPlayer2->GetModel()->SetPosition(m_pClient->GetPosition("P2"));
		if (m_pPlayer3->IsActive())
			m_pPlayer3->GetModel()->SetPosition(m_pClient->GetPosition("P3"));
		if (m_pPlayer4->IsActive())
			m_pPlayer4->GetModel()->SetPosition(m_pClient->GetPosition("P4"));

		m_packet.SerializePosition(POSITION_P, usernameC, "-", m_pPlayer1->GetModel()->GetPosition());
	}
	if (username == " P2") {
		
	//	if (!m_pClient->HasCollided())
			m_pPlayer2->SetPositions(m_fDeltaTick);

		if (m_pPlayer1->IsActive())
			m_pPlayer1->GetModel()->SetPosition(m_pClient->GetPosition("P1"));
		if (m_pPlayer3->IsActive())
			m_pPlayer3->GetModel()->SetPosition(m_pClient->GetPosition("P3"));
		if (m_pPlayer4->IsActive())
			m_pPlayer4->GetModel()->SetPosition(m_pClient->GetPosition("P4"));

		m_packet.SerializePosition(POSITION_P, usernameC, "-", m_pPlayer2->GetModel()->GetPosition());
	}
	if (username == " P3") {

	//	if (!m_pClient->HasCollided())
			m_pPlayer3->SetPositions(m_fDeltaTick);

		if (m_pPlayer1->IsActive())
			m_pPlayer1->GetModel()->SetPosition(m_pClient->GetPosition("P1"));
		if (m_pPlayer2->IsActive())
			m_pPlayer2->GetModel()->SetPosition(m_pClient->GetPosition("P2"));
		if (m_pPlayer4->IsActive())
			m_pPlayer4->GetModel()->SetPosition(m_pClient->GetPosition("P4"));

		m_packet.SerializePosition(POSITION_P, usernameC, "-", m_pPlayer3->GetModel()->GetPosition());
	}
	if (username == " P4") {

	//	if (!m_pClient->HasCollided())
			m_pPlayer4->SetPositions(m_fDeltaTick);

		if (m_pPlayer1->IsActive())
			m_pPlayer1->GetModel()->SetPosition(m_pClient->GetPosition("P1"));
		if (m_pPlayer3->IsActive())
			m_pPlayer3->GetModel()->SetPosition(m_pClient->GetPosition("P3"));
		if (m_pPlayer2->IsActive())
			m_pPlayer2->GetModel()->SetPosition(m_pClient->GetPosition("P2"));

		m_packet.SerializePosition(POSITION_P, usernameC, "-", m_pPlayer4->GetModel()->GetPosition());
	}

	m_pClient->SendData(m_packet.PacketData);

}

void GameScene::UpdateEnemiesForClient() {

	for (int x{ 0 }; x < 8; x++) {

		bool isEnemyActive = m_pClient->GetEnemies().at(x)->IsActive();
		m_enemies[x]->SetActive(isEnemyActive);
		vec3 pos = m_pClient->GetEnemies().at(x)->GetPosition();
		m_enemies[x]->GetModel()->SetPosition(pos);
	}
}

#pragma endregion UPDATE

#pragma region KEYBOARD
void GameScene::KeyDown(unsigned char key, int x, int y) {

	keyState[key] = BUTTON_DOWN;

}

void GameScene::KeyUp(unsigned char key, int x, int y) {
	keyState[key] = BUTTON_UP;
}
#pragma endregion KEYBOARD

#pragma region RENDER
/***********************
* GameScene render entities: Calls entity render on each entity stored in the gamescene vectors
* Parameters: none
* Return: void
********************/
void GameScene::RenderEntities() {

	if (m_iGameState == PLAY) {

		// Render player bullets
		if (!(m_pPlayer1->IsPlayerDead()))
			for (auto it = m_player1Bullets.begin(); it != m_player1Bullets.end(); it++) {
				if (((*it)->GetModel() != nullptr) && ((*it)->IsActive()))
					(*it)->Render();
			}

		if (!(m_pPlayer2->IsPlayerDead()))
			for (auto it = m_player2Bullets.begin(); it != m_player2Bullets.end(); it++) {
				if (((*it)->GetModel() != nullptr) && ((*it)->IsActive()))
					(*it)->Render();
			}

		if (!(m_pPlayer3->IsPlayerDead()))
			for (auto it = m_player3Bullets.begin(); it != m_player3Bullets.end(); it++) {
				if (((*it)->GetModel() != nullptr) && ((*it)->IsActive()))
					(*it)->Render();
			}

		if (!(m_pPlayer4->IsPlayerDead()))
			for (auto it = m_player4Bullets.begin(); it != m_player4Bullets.end(); it++) {
				if (((*it)->GetModel() != nullptr) && ((*it)->IsActive()))
					(*it)->Render();
			}

		// Render enemy bullets
		for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
			if (((*it)->GetModel() != nullptr) && ((*it)->IsActive()))
				(*it)->Render();
		}

		// Render players
		for (auto it = m_players.begin(); it != m_players.end(); it++) {
			if ((*it)->IsActive() && (!(*it)->IsPlayerDead()))
				(*it)->Render();
		}

		// Render map
		for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
			(*it)->Render();
		}

		// Render enemies
		for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
			if ((*it)->IsActive())
				(*it)->Render();
		}

		//// Render power ups
		//for (auto it = m_powerUps.begin(); it != m_powerUps.end(); it++) {
		//	if ((*it)->IsActive())
		//		(*it)->Render();
		//}
	}

}

/***********************
* GameScene render text: Calls text label render on each of the text labels stored in the game scene
* Parameters: none
* Return: void
********************/
void GameScene::RenderText() {

	// Render text
	for (auto textLabel = m_textLabels.begin(); textLabel != m_textLabels.end(); textLabel++) {

		if (m_iGameState == MENU) {

			if ((*textLabel)->GetTextType() == M_LABEL) {
				(*textLabel)->Render();
			}
			else if ((*textLabel)->GetTextType() == M_CURSOR) {

				(*textLabel)->setPosition(vec2((*textLabel)->GetPosition().x, m_iHeight - 300 - (50 * m_iCursorIndex)));
				(*textLabel)->Render();

			}

		}
		else if (m_iGameState == NETWORK_SELECT) {

			if (m_eNetworkEntityType == UNDEFINED) {

				if ((*textLabel)->GetTextType() == NS_LABEL) {
					(*textLabel)->Render();
				}
				else if ((*textLabel)->GetTextType() == NS_CURSOR) {

					(*textLabel)->setPosition(vec2((*textLabel)->GetPosition().x, m_iHeight - 300 - (50 * m_iCursorIndex)));
					(*textLabel)->Render();

				}

			}
			else if (m_eNetworkEntityType == SERVER) {

				if ((*textLabel)->GetTextType() == SERVER_LABEL) {
					(*textLabel)->Render();
				}

			}
			else if (m_eNetworkEntityType == CLIENT) {

				if ((*textLabel)->GetTextType() == CLIENT_LABEL) {
					(*textLabel)->Render();
				}
				else if ((*textLabel)->GetTextType() == CLIENT_CURSOR) {

					(*textLabel)->setPosition(vec2((*textLabel)->GetPosition().x, m_iHeight - 300 - (50 * m_iCursorIndex)));
					(*textLabel)->Render();

				}

			}

		}
		else if (m_iGameState == PLAY) {

			if ((*textLabel)->GetTextType() == P_AIDESCRIPTION) {

				if (m_iCurrentLevel < 8) {
					(*textLabel)->setText((*m_enemies.begin())->GetAIName());
					(*textLabel)->setPosition((*m_enemies.begin())->GetTextPosition());
				}

				else if (m_iCurrentLevel == 8) {
					(*textLabel)->setText("GAME ON");
					(*textLabel)->setPosition(vec2(658, 860));
				}

				else {
					(*textLabel)->setText("");
					(*textLabel)->setPosition(vec2(658, 860));
				}

				(*textLabel)->Render();

			}
			else if ((*textLabel)->GetTextType() == P_SCORE) {
				//	(*textLabel)->setText((m_pPlayer->GetScore()));
				(*textLabel)->setText(GetScore());
				(*textLabel)->Render();
			}

			else if ((*textLabel)->GetTextType() == P_LEVEL) {
				(*textLabel)->setText(std::to_string(m_iCurrentLevel));
				(*textLabel)->Render();
			}

			else if ((*textLabel)->GetTextType() == P_LIVES) {
				//	(*textLabel)->setText(m_pPlayer->GetLives());
				(*textLabel)->setText(GetLives());
				(*textLabel)->Render();
			}
			else if ((*textLabel)->GetTextType() == P_LABEL) {
				(*textLabel)->Render();
			}

		}
		else if (m_iGameState == GAME_OVER) {

			if ((*textLabel)->GetTextType() == G_LABEL) {
				(*textLabel)->Render();
			}

			else if ((*textLabel)->GetTextType() == G_SCORE) {

				//	(*textLabel)->setText((m_pPlayer->GetScore()));
				(*textLabel)->setText(GetScore());
				(*textLabel)->Render();
			}

			else if ((*textLabel)->GetTextType() == G_LEVEL) {

				(*textLabel)->setText(std::to_string(m_iCurrentLevel));
				(*textLabel)->Render();
			}
		}
		else if (m_iGameState == CREDITS) {

			if ((*textLabel)->GetTextType() == C_LABEL) {
				(*textLabel)->Render();
			}

		}

	}

}
#pragma endregion RENDER

#pragma region SOUNDS
bool GameScene::InitFmod() {
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_pAudioMgr);
	if (result != FMOD_OK) return false;

	result = m_pAudioMgr->init(50, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) return false;

	return true;
}

const bool GameScene::LoadAudio() {
	FMOD_RESULT result;

	// Background music
	result = m_pAudioMgr->createSound("Assets//Sounds//Background_Music.mp3", FMOD_DEFAULT, 0, &m_pBGMusic);
	m_pBGMusic->setMode(FMOD_LOOP_NORMAL);

	// Sound effects
	result = m_pAudioMgr->createSound("Assets//Sounds//die_1.wav", FMOD_DEFAULT, 0, &m_pDieSound);
	result = m_pAudioMgr->createSound("Assets//Sounds//hit_1.wav", FMOD_DEFAULT, 0, &m_pHitSound);
	result = m_pAudioMgr->createSound("Assets//Sounds//powerup_1.wav", FMOD_DEFAULT, 0, &m_pPowerUpSound);
	result = m_pAudioMgr->createSound("Assets//Sounds//scroll_1.wav", FMOD_DEFAULT, 0, &m_pScrollSound);
	result = m_pAudioMgr->createSound("Assets//Sounds//shoot_1.wav", FMOD_DEFAULT, 0, &m_pShootSound);
	result = m_pAudioMgr->createSound("Assets//Sounds//select_1.wav", FMOD_DEFAULT, 0, &m_pSelectSound);

	return true;
}
#pragma endregion SOUNDS

#pragma region NETWORKING
// Creates the network by type
void GameScene::CreateNetwork() {

	m_pNetwork = new CNetwork();

	m_pcPacketData = 0; //A local buffer to receive packet data into
	m_pcPacketData = new char[MAX_MESSAGE_LENGTH];

	strcpy_s(m_pcPacketData, strlen("") + 1, "");

	char _cIPAddress[MAX_ADDRESS_LENGTH]; // An array to hold the IP Address as a string

	unsigned char _ucChoice;

	m_pNetwork->StartUp();

	if (!m_pNetwork->GetInstance().Initialise(m_eNetworkEntityType)) {
		std::cout << "Unable to initialise the Network........Press any key to continue......";
		_getch();
	}

	//Run receive on a separate thread so that it does not block the main client thread.
	if (m_eNetworkEntityType == CLIENT) //if network entity is a client
	{

		m_pClient = static_cast<CClient*>(m_pNetwork->GetInstance().GetNetworkEntity());

		if (m_pClient->GetPorts().size() == 0) {

			m_pTextLabel = new TextLabel(CLIENT_LABEL, "NO SERVERS FOUND", "Assets//Fonts//freeagent.ttf");
			m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 300));
			m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			AddText(m_pTextLabel);

		}
		else {

			m_pTextLabel = new TextLabel(CLIENT_LABEL, "SERVERS FOUND", "Assets//Fonts//freeagent.ttf");
			m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight - 100));
			m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			AddText(m_pTextLabel);

			m_iClientChoices = m_pClient->GetIPAddresses().size();

			m_pTextLabel = new TextLabel(CLIENT_CURSOR, ">", "Assets//Fonts//freeagent.ttf");
			m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 300, m_iHeight - 300));
			m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			AddText(m_pTextLabel);

			for (unsigned int i = 0; i < m_iClientChoices; i++) {

				std::string s = m_pClient->GetIPAddresses().at(i);
				std::string p = ToString(ntohs(m_pClient->GetPorts().at(i).sin_port));
				s += ":";
				s += p;

				m_pTextLabel = new TextLabel(CLIENT_LABEL, s, "Assets//Fonts//opensans.ttf");
				m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight - 300 - (50 * i)));
				m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
				AddText(m_pTextLabel);

			}

		}

		m_ReceiveClientThread = std::thread(&CClient::ReceiveData, m_pClient, std::ref(m_pcPacketData));

	}

	if (m_eNetworkEntityType == SERVER) //if network entity is a server
	{

		m_pServer = static_cast<CServer*>(m_pNetwork->GetInstance().GetNetworkEntity());

		m_pTextLabel = new TextLabel(SERVER_LABEL, "CREATED SERVER AT", "Assets//Fonts//freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 450, m_iHeight - 100));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(SERVER_LABEL, m_pServer->GetServerAddress(), "Assets//Fonts//opensans.ttf");
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 450, m_iHeight - 200));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_ReceiveServerThread = std::thread(&CServer::ReceiveData, m_pServer, std::ref(m_pcPacketData));

	}

}

void GameScene::UpdateNetwork() {

	if (m_eNetworkEntityType == CLIENT) {

		m_pClient = static_cast<CClient*>(m_pNetwork->GetInstance().GetNetworkEntity());

		// Call the process data function
		if (!m_pClient->GetWorkQueue()->empty()) {

			m_pClient->GetWorkQueue()->pop(m_pcPacketData);
			m_pClient->ProcessData(m_pcPacketData);

		}

	}
	else if (m_eNetworkEntityType == SERVER) {

		m_pServer = static_cast<CServer*>(m_pNetwork->GetInstance().GetNetworkEntity());

		// Call the process data function
		if (!m_pServer->GetWorkQueue()->empty()) {

			m_pServer->GetWorkQueue()->pop(m_pcPacketData);
			m_pServer->ProcessData(m_pcPacketData);

		}

	}
	// No defined network type
	else {

		return;

	}

}

EEntityType GameScene::GetNetworkEntityType() {
	return m_eNetworkEntityType;
}

void GameScene::SetNetworkEntityType(EEntityType _type) {
	m_eNetworkEntityType = _type;
}

#pragma endregion NETWORKING