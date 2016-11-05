//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : gameScene.cpp
// Description : Game scene implementation file - singleton game scene class that creates the models
// Mail : Mitchell.Currie@mediadesignschool.com
//

#include <iostream>
#include "glew.h"
#include "freeglut.h"

#include "gameScene.h"

// Static variables
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
int GameScene::m_iMenuIndex = 0;
const int CHOICES = 3;

// Keyboard input
unsigned char GameScene::keyState[255];

GameScene::GameScene() :
	m_iWidth(0),
	m_iHeight(0),
	m_iCurrentLevel(1),
	m_bLevelComplete(false),
	m_fDeltaTick(0.0f) {

	m_pClock = new CClock;
	m_pBullet = nullptr;
	m_pPlayer = nullptr;
	m_pEnemy = nullptr;
	m_pMap = nullptr;
	m_pTextLabel = nullptr;
	m_pPowerUp = nullptr;

	m_iGameState = MENU;
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
}

void GameScene::CreateEntities() {
	// Player Cube
	m_pPlayer = new Entity;
	m_pPlayer->SetAsPlayer();
	m_pPlayer->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(-15.0f, 0.0f, -15.0f), NONENEMY, 8.0f, "PlayerTexture.jpg");
	AddEntity(m_pPlayer);

	// Map Quad
	m_pMap = new Entity;
	m_pMap->Initialise(MAP, QUAD, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "Map.jpg");
	AddEntity(m_pMap);

	// Player Bullets
	for (int x{ 0 }; x < 10; x++) {
		m_pBullet = new Entity;
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "PlayerBullet.png");
		AddPlayerBullet(m_pBullet);
	}

	// Enemy Bullets
	for (int y{ 0 }; y < 40; y++) {
		m_pBullet = new Entity;
		m_pBullet->SetAsEnemyBullet();
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f, "EnemyBullet.png");
		AddEnemyBullet(m_pBullet);
	}

	////////////////// Enemies //////////////////////////////////

	float fMaxVelocity = 0.05f;

	// 1
	m_pEnemy = new Entity;
	m_pEnemy->SetLeader(true);
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.1f, 0.0f, 0.1f), WANDER, fMaxVelocity, "EnemyTexture.jpg"); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg"); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-2.0f, 0.0f, 2.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg"); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(2.0f, 0.0f, -2.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	// 6
	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-6.0f, 0.0f, -5.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 0.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-2.0f, 0.0f, -2.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(2.0f, 0.0f, 2.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	// 11
	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 0.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 5.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, -5.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 0.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 0.0f), WANDER, fMaxVelocity, "EnemyTexture.jpg");
	AddEnemy(m_pEnemy);

	// 15 enemies

	// Extras for spawning in - 20 enemies 

	for (int z{ 0 }; z < 20; z++) {
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.1f, 0.0f, 0.1f), SEEK, fMaxVelocity, "EnemyTexture.jpg");
		AddEnemy(m_pEnemy);
	}

	// 35 Enemies total

	//////////////////////////////////////////////////////////////

	// Power Ups

	m_pPowerUp = new Entity;
	m_pPowerUp->Initialise(POWERUP, PYRAMID, 18, m_Camera, vec3(-2.5f, 0.0f, -2.0f), NONENEMY, 0.03f, "GreenPattern.png");
	m_pPowerUp->SetPowerUpType(SPEEDBOOST);
	AddPowerUp(m_pPowerUp);

	m_pPowerUp = new Entity;
	m_pPowerUp->Initialise(POWERUP, PYRAMID, 18, m_Camera, vec3(-2.5f, 0.0f, -2.0f), NONENEMY, 0.03f, "PinkPattern.png");
	m_pPowerUp->SetPowerUpType(FASTFIRING);
	AddPowerUp(m_pPowerUp);

	m_pPowerUp = new Entity;
	m_pPowerUp->Initialise(POWERUP, PYRAMID, 18, m_Camera, vec3(-2.5f, 0.0f, -2.0f), NONENEMY, 0.03f, "WhitePattern.png");
	m_pPowerUp->SetPowerUpType(ENEMYFLEE);
	AddPowerUp(m_pPowerUp);
}
 
void GameScene::CreateTextFields() {

	/// MENU
	{
		m_pTextLabel = new TextLabel(M_LABEL, "Robotron", "freeagent.ttf");
		m_pTextLabel->setScale(1.2f);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight - 100));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(M_CURSOR, ">", "freeagent.ttf");
		m_pTextLabel->setScale(1);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 200, m_iHeight - 300));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(M_LABEL, "PLAY", "freeagent.ttf");
		m_pTextLabel->setScale(1);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 300));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(M_LABEL, "CREDITS", "freeagent.ttf");
		m_pTextLabel->setScale(1);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 350));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(M_LABEL, "QUIT", "freeagent.ttf");
		m_pTextLabel->setScale(1);
		m_pTextLabel->setPosition(glm::vec2(m_iWidth / 2 - 100, m_iHeight - 400));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);
	}

	/// PLAY
	{
		m_pTextLabel = new TextLabel(P_LABEL, "Robotron", "freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(15, 860));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LABEL, "Score:", "freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(15, 820));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_AIDESCRIPTION, "", "freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(070, 860));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_SCORE, "", "freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(250, 820));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LABEL, "Lives:", "freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(1200, 820));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LABEL, "Level:", "freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(1200, 860));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LEVEL, "", "freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(1430, 860));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);

		m_pTextLabel = new TextLabel(P_LIVES, "", "freeagent.ttf");
		m_pTextLabel->setPosition(glm::vec2(1430, 820));
		m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		AddText(m_pTextLabel);
	}

	/// GAME OVER
	{

	}

	/// CREDITS
	{

	}

}
#pragma endregion CREATE

#pragma region SETTERS
void GameScene::SetPlayerAlive() {
	m_pPlayer->SetToAlive();
}

void GameScene::SetLevel(int _iLevel) {
	m_iCurrentLevel = _iLevel;
}

void GameScene::SetLevelComplete(bool _IsComplete) {
	m_bLevelComplete = _IsComplete;
}

void GameScene::SetAllBulletsInactive() {
	for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++) {
		(*it)->SetActivity(false);
		//(*it)->ResetVelocityForBullets();		
	}

	for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
		(*it)->SetActivity(false);
		//(*it)->ResetVelocityForBullets();		
	}
}

void GameScene::AddCamera(Camera _camera) {
	m_Camera = _camera;
}

void GameScene::AddEntity(Entity* _entity) {
	m_entities.push_back(_entity);
}

void GameScene::AddEnemy(Entity* _entity) {
	m_enemies.push_back(_entity);
}

void GameScene::AddPlayerBullet(Entity* _entity) {
	m_playerBullets.push_back(_entity);
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

Entity* GameScene::GetPlayer() {
	return m_pPlayer;
}

AIBehaviour GameScene::GetRandomBehaviour() {
	//  Not currently used

	//	srand(time(NULL));
	int iBehaviour = rand() % 3;

	switch (iBehaviour) {
		case 0:
		{
			return SEEK;
		}
		break;

		case 1:
		{
			return PURSUE;
		}
		break;

		case 2:
		{
			return WANDER;
		}
		break;

		default:
		{
			return NONENEMY;
		}
		break;
	}
}

bool GameScene::IsGameOver() {
	if (m_pPlayer->GetPlayerLives() < 0)
		return true;

	else
		return false;
}
#pragma endregion GETTERS

#pragma region ENTITY_CHECK
void GameScene::SetPositions(float _fDeltaTick) {
	//Player, Map
	for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
		(*it)->SetPositions(_fDeltaTick);
	}

	// Enemies
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}

	//Player Bullets
	for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++) {
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

void GameScene::UpdateEntities() {
	// Player back to starting position
	m_pPlayer->GetModel()->ResetToStartingPosition();

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
		(*it)->SetActivity(false);
		(*it)->SetLeader(false);
		(*it)->SetModelOutsideMap(false);
		(*it)->SetMaxVelocity(0.05f);
	}

	// Reset AI behaviours - from fleeing powerup
	ResetEnemyBehaviours();

	// Reset powerups and set to inactive.
	for (auto it = m_powerUps.begin(); it != m_powerUps.end(); it++) {
		(*it)->GetModel()->ResetToBeDeleted();
		(*it)->SetActivity(false);
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
			m_enemies.at(x)->SetActivity(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 2) {
		for (int x{ 0 }; x < 7; x++) {
			m_enemies.at(x)->SetAIBehaviour(EVADE);
			m_enemies.at(x)->SetActivity(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 3) {
		for (int x{ 0 }; x < 8; x++) {
			m_enemies.at(x)->SetAIBehaviour(WANDER);
			m_enemies.at(x)->SetActivity(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 4) {
		for (int x{ 0 }; x < 6; x++) {
			m_enemies.at(x)->SetAIBehaviour(SEEK);
			m_enemies.at(x)->SetActivity(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 5) {
		for (int x{ 0 }; x < 8; x++) {
			m_enemies.at(x)->SetAIBehaviour(LEADERFOLLOW);
			m_enemies.at(x)->SetActivity(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 6) {
		for (int x{ 0 }; x < 8; x++) {
			m_enemies.at(x)->SetAIBehaviour(FLOCK);
			m_enemies.at(x)->SetActivity(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else if (m_iCurrentLevel == 7) {
		for (int x{ 0 }; x < 6; x++) {
			m_enemies.at(x)->SetAIBehaviour(PURSUE);
			m_enemies.at(x)->SetActivity(true);
			Entity::IncrementEnemyCounter();
		}
	}

	else // After level 7
	{
		float fMaxVelocity = 0.05f + ((static_cast<float>(m_iCurrentLevel)) / 375.0f);
		//	float fMaxVelocity = 0.0001f;  // To test positions
		//	float fMaxVelocity = 0.05f;

		for (int x{ 0 }; x < 2; x++) {
			m_enemies.at(x)->SetAIBehaviour(SEEK);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
			Entity::IncrementEnemyCounter();
		}

		for (int x{ 2 }; x < 4; x++) {
			m_enemies.at(x)->SetAIBehaviour(PURSUE);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
			Entity::IncrementEnemyCounter();
		}

		for (int x{ 4 }; x < 11; x++) {
			m_enemies.at(x)->SetAIBehaviour(WANDER);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
			Entity::IncrementEnemyCounter();
		}

		for (int x{ 11 }; x < 15; x++) {
			m_enemies.at(x)->SetAIBehaviour(FLOCK);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
			Entity::IncrementEnemyCounter();
		}
	}
}

void GameScene::CheckBullets() {
	//////// Player Bullets  //////////////////
	// Bullet fired
	if (Entity::IsBulletFired()) {
		m_playerBullets.at(Entity::GetPlayerBulletCounter())->SetActivity(true); // set next bullet in vector to active
		m_playerBullets.at(Entity::GetPlayerBulletCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
		m_playerBullets.at(Entity::GetPlayerBulletCounter())->SetModelPosition(m_pPlayer->GetModel()->GetPlayerPosition()); // Set position to players current position
		Entity::IncrementPlayerBulletCounter(); // increment the bullet counter for the next bullet fired
	}

	// Check if any bullets need to be deleted - either through collision with object or edge of map
	for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++) {
		if ((*it)->ToDelete()) {
			(*it)->SetActivity(false); // set to inactive
			(*it)->ResetVelocityForPlayerBullets(); // reset velocity
		}
	}

	//////// Enemy Bullets  //////////////////

	// Randomise whether each enemy should fire

	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		if ((rand() % 130 == 1) && (*it)->IsActive() && (!(*it)->GetModel()->IsOutsideMap())) // 1/130 chance of firing
		{
			m_enemyBullets.at(Entity::GetEnemyBulletCounter())->SetActivity(true); // set next bullet in vector to active
			m_enemyBullets.at(Entity::GetEnemyBulletCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
			m_enemyBullets.at(Entity::GetEnemyBulletCounter())->SetModelPosition((*it)->GetModel()->GetPosition()); // Set position to enemies current position
			Entity::IncrementEnemyBulletCounter();
		}
	}

	// Check if any bullets need to be deleted - either through collision with object or edge of map 
	for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
		if ((*it)->ToDelete()) {
			(*it)->SetActivity(false); // set to inactive
			(*it)->VelocityToZero(); // reset velocity
		}
	}
}

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

			m_enemies.at(Entity::GetEnemyCounter())->SetActivity(true); // set next bullet in vector to active
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
			(*it)->SetActivity(false); // set to inactive
		}
	}
}

void GameScene::CheckPowerUps() {
	int i = rand() % 3;

	// Creating power ups
	if ((rand() % 400 == 39) && !AllPowerUpsActive())   // change back to 1 in 400
	{
		while (m_powerUps.at(i)->IsActive()) {
			i = rand() % 3;
		}

		m_powerUps.at(i)->SetActivity(true); // set random power up to active
		m_powerUps.at(i)->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
		m_powerUps.at(i)->SetModelPosition(GetRandomMapPosition()); // Set position		
	}

	// Check if any powerups need to be deleted - either through collision with bullet or player
	for (auto it = m_powerUps.begin(); it != m_powerUps.end(); it++) {
		if ((*it)->ToDelete()) {
			(*it)->SetActivity(false); // set to inactive
		}
	}

	// Speed boosts
	for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
		if ((*it)->GetEntityType() == PLAYER) {
			m_durationSpeedBoostTimer = (std::clock() - m_startSpeedBoostTimer) / (double)CLOCKS_PER_SEC;

			if (m_durationSpeedBoostTimer > 5.0f) {
				(*it)->ResetMaxVelocity();
				(*it)->SetSpedUp(false);
				m_startSpeedBoostTimer = std::clock();
			}
		}
	}

	// Fast firing
	for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
		if ((*it)->GetEntityType() == PLAYER) {
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

	if (m_pPlayer->GetScoreCounter() >= 1000) {
		m_pPlayer->AddExtraLife();
		m_pPlayer->ResetScoreCounter();
	}
}
#pragma endregion ENTITY_CHECK

#pragma region COLLISION
void GameScene::CheckCollisions() {
	// Enemy and player collisions
	for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
		for (auto it2 = m_enemies.begin(); it2 != m_enemies.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.2f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.2f) &&
				((*it)->GetEntityType() == PLAYER) && (*it2)->IsActive()) {
				(*it)->SetToDead();
				(*it)->ReducePlayerLives();
				return;
			}
		}
	}

	// Enemy bullet and player collisions
	for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
		for (auto it2 = m_enemyBullets.begin(); it2 != m_enemyBullets.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
				((*it)->GetEntityType() == PLAYER) && ((*it2)->IsActive())) {
				(*it)->SetToDead();
				(*it)->ReducePlayerLives();
				return;

				//break;
			}
		}
	}

	// Player bullet and enemy collisions
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
		for (auto it2 = m_playerBullets.begin(); it2 != m_playerBullets.end(); it2++) {
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

				m_pPlayer->AddToScore(10);

				break;
			}
		}
	}

	// Player bullet and enemy bullet collisions
	for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
		for (auto it2 = m_playerBullets.begin(); it2 != m_playerBullets.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 0.5f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 0.5f) &&
				((*it2)->IsActive()) && ((*it)->IsActive())) {
				(*it)->GetModel()->SetToBeDeleted(); // set to delete enemy bullet
				(*it2)->GetModel()->SetToBeDeleted(); // set to delete player bullet

													  //break;
			}
		}
	}

	// Player and power up collisions
	for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
		for (auto it2 = m_powerUps.begin(); it2 != m_powerUps.end(); it2++) {
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
				((*it)->GetEntityType() == PLAYER) && ((*it2)->IsActive())) {
				(*it2)->GetModel()->SetToBeDeleted();

				m_pPlayer->AddToScore(20);

				if ((*it2)->GetPowerUpType() == SPEEDBOOST) {
					(*it)->SpeedUp();
					(*it)->SetSpedUp(true);
					m_startSpeedBoostTimer = std::clock();
				}

				if ((*it2)->GetPowerUpType() == FASTFIRING) {
					(*it)->FastFire();
					(*it)->SetFastFire(true);
					m_startFastFireTimer = std::clock();
				}

				if ((*it2)->GetPowerUpType() == ENEMYFLEE) {
					SetEnemiesToFlee();
					m_startEnemyFleeTimer = std::clock();
				}

				return;

				//break;
			}
		}
	}

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
#pragma endregion LEVELS

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
void GameScene::Update() {

	GetClock()->Process();
	m_fDeltaTick = GetClock()->GetDeltaTick();

	if (m_iGameState == MENU) {

		if (keyState[(unsigned char)3] == BUTTON_DOWN || keyState[(unsigned char)13] == BUTTON_DOWN) {

			if (m_iMenuIndex == 0) {

				m_pPlayer->ResetPlayerLives();
				m_pPlayer->ResetScore();
				m_iCurrentLevel = 1;
				UpdateEntities();
				m_iGameState = PLAY;

			}
			else if (m_iMenuIndex == 1) {

				m_iGameState = CREDITS;

			}
			else if (m_iMenuIndex == 2) {

				glutDestroyWindow(1);

			}

			Sleep(100);

		}
		else if (keyState[(unsigned char)'w'] == BUTTON_DOWN) {

			m_iMenuIndex--;
			Sleep(100);

		}
		else if (keyState[(unsigned char)'s'] == BUTTON_DOWN) {

			m_iMenuIndex++;
			Sleep(100);

		}

		if (m_iMenuIndex < 0) {

			m_iMenuIndex = CHOICES - 1;

		}
		else if (m_iMenuIndex >= CHOICES) {

			m_iMenuIndex = 0;

		}

	}
	else if (m_iGameState == PLAY) {

		if (keyState[(unsigned char)27] == BUTTON_DOWN) {

			m_iGameState = MENU;
			Sleep(100);

		}

		if (GetPlayer()->IsPlayerDead()) {
			SetPlayerAlive();  // Set player back to alive
			SetAllBulletsInactive(); // To stop bullet rendering

			if (IsGameOver()) {
				SetLevel(1);  // go back to level one
				GetPlayer()->ResetPlayerLives();
				GetPlayer()->ResetScore();
			}

			UpdateEntities(); // restart level		
		}

		else if (IsLevelComplete() && (m_iCurrentLevel > 0)) {
			SetAllBulletsInactive(); // To stop bullet rendering
			NextLevel();
			UpdateEntities();   // start the next level
			SetLevelComplete(false);
		}

		CheckEnemies();  // Check if any enemies need to be deactivated - if set "to delete"
		CheckBullets();  // Check if any bullets need to be deactivated - if set "to delete"
		CheckPowerUps();
		SetPositions(m_fDeltaTick);  // Set all entity positions	
		CheckCollisions();

	}
	else if (m_iGameState == GAME_OVER) {



	}
	else if (m_iGameState == CREDITS) {



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
void GameScene::RenderEntities() {

	if (m_iGameState == PLAY) {

		// Render player bullets
		for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++) {
			if (((*it)->GetModel() != nullptr) && ((*it)->IsActive()))
				(*it)->Render();
		}

		// Render enemy bullets
		for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); it++) {
			if (((*it)->GetModel() != nullptr) && ((*it)->IsActive()))
				(*it)->Render();
		}

		// Render player and map
		for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
			(*it)->Render();
		}

		// Render enemies
		for (auto it = m_enemies.begin(); it != m_enemies.end(); it++) {
			if ((*it)->IsActive())
				(*it)->Render();
		}

		// Render power ups
		for (auto it = m_powerUps.begin(); it != m_powerUps.end(); it++) {
			if ((*it)->IsActive())
				(*it)->Render();
		}

	}

}

void GameScene::RenderText() {

	// Render text
	for (auto textLabel = m_textLabels.begin(); textLabel != m_textLabels.end(); textLabel++) {

		if (m_iGameState == MENU) {

			if ((*textLabel)->GetTextType() == M_LABEL) {
				(*textLabel)->Render();
			}
			else if ((*textLabel)->GetTextType() == M_CURSOR) {

				(*textLabel)->setPosition(vec2((*textLabel)->GetPosition().x, m_iHeight - 300 - (50 * m_iMenuIndex)));
				(*textLabel)->Render();

			}

		}
		else if (m_iGameState == PLAY) {

			if ((*textLabel)->GetTextType() == P_AIDESCRIPTION) {

				if (m_iCurrentLevel < 8) {
					(*textLabel)->setText((*m_enemies.begin())->GetAIName());
					(*textLabel)->setPosition((*m_enemies.begin())->GetTextPosition());
				}

				else {
					(*textLabel)->setText("GAME ON");
					(*textLabel)->setPosition(vec2(658, 860));
				}

				(*textLabel)->Render();

			}
			else if ((*textLabel)->GetTextType() == P_SCORE) {
				(*textLabel)->setText((m_pPlayer->GetScore()));
				(*textLabel)->Render();
			}

			else if ((*textLabel)->GetTextType() == P_LEVEL) {
				(*textLabel)->setText(std::to_string(m_iCurrentLevel));
				(*textLabel)->Render();
			}

			else if ((*textLabel)->GetTextType() == P_LIVES) {
				(*textLabel)->setText(m_pPlayer->GetLives());
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

		}
		else if (m_iGameState == CREDITS) {

			if ((*textLabel)->GetTextType() == C_LABEL) {
				(*textLabel)->Render();
			}

		}

	}


}
#pragma endregion RENDER