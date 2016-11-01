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

GameScene::GameScene()
	//	: m_pClock(0)
{
	m_pClock = new CClock;
	m_pBullet = nullptr;
	m_pPlayer = nullptr;
	m_pEnemy = nullptr;
	m_pMap = nullptr;
	m_pTextLabel = nullptr;
	m_iCurrentLevel = 1;
	m_bLevelComplete = false;
	m_fDeltaTick = 0.0f;
}
GameScene::~GameScene()
{
	delete m_pClock;
	m_pClock = 0;
}

GameScene& GameScene::GetInstance()
{
	if (s_pGameScene == 0)
	{
		s_pGameScene = new GameScene();
	}

	return (*s_pGameScene);
}

void GameScene::AddCamera(Camera _camera)
{
	m_Camera = _camera;
}

void GameScene::AddEntity(Entity* _entity)
{
	m_entities.push_back(_entity);
}

void GameScene::AddEnemy(Entity* _entity)
{
	m_enemies.push_back(_entity);
}

void GameScene::AddPlayerBullet(Entity* _entity)
{
	m_playerBullets.push_back(_entity);
}

void GameScene::AddText(TextLabel* _text)
{
	m_textLabels.push_back(_text);
}

void GameScene::CheckBullets()
{	
	// Bullet fired
	if (Entity::IsBulletFired())
	{
		m_playerBullets.at(m_pPlayer->GetBulletCounter())->SetActivity(true); // set next bullet in vector to active
		m_playerBullets.at(m_pPlayer->GetBulletCounter())->GetModel()->ResetToBeDeleted(); // set "tobedeleted" to false so its not immediately deleted
		m_playerBullets.at(m_pPlayer->GetBulletCounter())->SetModelPosition(m_pPlayer->GetModel()->GetPlayerPosition()); // Set position to players current position
		m_pPlayer->IncrementBulletCounter(); // increment the bullet counter for the next bullet fired
	}

	// Check if any bullets need to be deleted - either through collision with object or edge of map
	for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++)
	{
		if ((*it)->ToDelete())
		{
			(*it)->SetActivity(false); // set to inactive
			(*it)->ResetVelocityForBullets(); // reset velocity
		}
	}
}

void GameScene::CheckEnemies()
{
	// Check if any enemies need to be deleted - either through collision with bullet or player
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++)
	{
		if ((*it)->ToDelete())
		{
			(*it)->SetActivity(false); // set to inactive
		}
	}
}

void GameScene::RenderEntities()
{
	// Render player bullets
	for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++)
	{
		if (((*it)->GetModel() != nullptr) && ((*it)->IsActive()))
			(*it)->Render();
	}

	// Redner player and map
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{		
		(*it)->Render();	
	}

	// Render enemies
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++)
	{
		if ((*it)->IsActive())
			(*it)->Render();
	}
}

void GameScene::RenderText()
{
	// Render text
	for (auto it = m_textLabels.begin(); it != m_textLabels.end(); it++)
	{
		if ((*it)->GetTextType() == AIDESCRIPTION)
		{
			if (m_iCurrentLevel < 8)
			{
				(*it)->setText((*m_enemies.begin())->GetAIName());
				(*it)->setPosition((*m_enemies.begin())->GetTextPosition());
			}

			else
			{
				(*it)->setText("Multiple");
				(*it)->setPosition(vec2(658, 860));
			}		
		}

		else if ((*it)->GetTextType() == SCORE)
		{
			(*it)->setText((*m_entities.begin())->GetScore());
			//it->setPosition(m_entities.begin()->GetTextPosition());
		}

		else if ((*it)->GetTextType() == LEVEL)
		{
			(*it)->setText(std::to_string(m_iCurrentLevel));
		}

		else if ((*it)->GetTextType() == LIVES)
		{
			(*it)->setText((*m_entities.begin())->GetLives());
		}
		
		(*it)->Render();
	}
}

void GameScene::CreateEntities()
{
	// Player Cube
	m_pPlayer = new Entity;
	m_pPlayer->SetAsPlayer();
	m_pPlayer->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(-12.0f, 0.0f, 12.0f), NONENEMY, 0.0f);	
	AddEntity(m_pPlayer);

	// Map Quad
	m_pMap = new Entity;
	m_pMap->Initialise(MAP, QUAD, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f);
	AddEntity(m_pMap);

	// Player Bullets
	for (int x{ 0 }; x < 10; x++)
	{
		m_pBullet = new Entity;
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), NONENEMY, 0.0f);
		AddPlayerBullet(m_pBullet);
	}

	////////////////// Enemies //////////////////////////////////
 
	float fMaxVelocity = 0.05f;

	// 1
	m_pEnemy = new Entity;
	m_pEnemy->SetAsLeader();
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.1f, 0.0f, 0.1f), WANDER, fMaxVelocity); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), WANDER, fMaxVelocity); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-2.0f, 0.0f, 2.0f), WANDER, fMaxVelocity); //GetRandomBehaviour()
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(2.0f, 0.0f, -2.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	// 6
	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 0.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-2.0f, 0.0f, -2.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(2.0f, 0.0f, 2.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	// 11
	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 0.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 5.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, -5.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 0.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	m_pEnemy = new Entity;
	m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 0.0f), WANDER, fMaxVelocity);
	AddEnemy(m_pEnemy);

	// 15 enemies


	//////////////////////////////////////////////////////////////


	//// Pyramid
	//Model ModelPyramid;
	//vec3 positionPyramid(-2.5f, 1.9f, -2.0f);
	//ModelPyramid.Initialise(PYRAMID, 18, m_Camera, positionPyramid);
	//AddModel(ModelPyramid);

	//// Octagon
	//Model ModelOctagon;
	//vec3 positionOctagon(0.0f, 0.0f, 0.0f);
	//ModelOctagon.Initialise(OCTAGON, 18, m_Camera, positionOctagon);
	//AddModel(ModelOctagon);
}

void GameScene::UpdateEntities()
{
	// Player back to starting position
	m_pPlayer->GetModel()->ResetToStartingPosition();


	// Reset AI positions and set to inactive.
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++)
	{
		(*it)->GetModel()->ResetToStartingPosition();
		(*it)->SetActivity(false);
	}

	// Spawn enemies based on level
	if (m_iCurrentLevel == 1)
	{
		for (int x{ 0 }; x < 5; x++) // Number of enemies
		{
			m_enemies.at(x)->SetAIBehaviour(FLEE);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}
	}

	else if (m_iCurrentLevel == 2)
	{
		for (int x{ 0 }; x < 5; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(EVADE);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}
	}

	else if (m_iCurrentLevel == 3)
	{
		for (int x{ 0 }; x < 6; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(WANDER);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}
	}

	else if (m_iCurrentLevel == 4)
	{
		for (int x{ 0 }; x < 5; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(SEEK);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}
	}

	else if (m_iCurrentLevel == 5)
	{
		for (int x{ 0 }; x < 6; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(LEADERFOLLOW);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}
	}

	else if (m_iCurrentLevel == 6)
	{
		for (int x{ 0 }; x < 6; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(FLOCK);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}
	}

	else if (m_iCurrentLevel == 7)
	{
		for (int x{ 0 }; x < 6; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(PURSUE);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}
	}

	else // After level 7
	{
		float fMaxVelocity = 0.05f + ((static_cast<float>(m_iCurrentLevel)) / 350.0f);
		//	float fMaxVelocity = 0.0001f;  // To test positions
		//	float fMaxVelocity = 0.05f;

		for (int x{ 0 }; x < 3; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(SEEK);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}

		for (int x{ 3 }; x < 7; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(PURSUE);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}

		for (int x{ 7 }; x < 12; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(WANDER);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}

		for (int x{ 12 }; x < 15; x++)
		{
			m_enemies.at(x)->SetAIBehaviour(FLOCK);
			m_enemies.at(x)->SetActivity(true);
			m_enemies.at(x)->SetMaxVelocity(fMaxVelocity);
			m_enemies.at(x)->GetModel()->ResetToBeDeleted();
		}
	}
}

void GameScene::CreateText()
{
	/// Text
	m_pTextLabel = new TextLabel(STATIC, "Robotron", "freeagent.ttf");
	m_pTextLabel->setPosition(glm::vec2(15, 860));
	m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	AddText(m_pTextLabel);

	m_pTextLabel = new TextLabel(STATIC, "Score:", "freeagent.ttf");
	m_pTextLabel->setPosition(glm::vec2(1200, 860));
	m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	AddText(m_pTextLabel);

	m_pTextLabel = new TextLabel(AIDESCRIPTION, "", "freeagent.ttf");
	m_pTextLabel->setPosition(glm::vec2(700, 860));
	m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	AddText(m_pTextLabel);

	m_pTextLabel = new TextLabel(SCORE, "", "freeagent.ttf");
	m_pTextLabel->setPosition(glm::vec2(1430, 860));
	m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	AddText(m_pTextLabel);

	m_pTextLabel = new TextLabel(STATIC, "Lives:", "freeagent.ttf");
	m_pTextLabel->setPosition(glm::vec2(1200, 820));
	m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	AddText(m_pTextLabel);

	m_pTextLabel = new TextLabel(STATIC, "Level:", "freeagent.ttf");
	m_pTextLabel->setPosition(glm::vec2(1200, 780));
	m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	AddText(m_pTextLabel);

	m_pTextLabel = new TextLabel(LEVEL, "", "freeagent.ttf");
	m_pTextLabel->setPosition(glm::vec2(1430, 780));
	m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	AddText(m_pTextLabel);

	m_pTextLabel = new TextLabel(LIVES, "", "freeagent.ttf");
	m_pTextLabel->setPosition(glm::vec2(1430, 820));
	m_pTextLabel->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	AddText(m_pTextLabel);
}

CClock* GameScene::GetClock()
{
	return m_pClock;
}

void GameScene::SetPositions(float _fDeltaTick)
{
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{
		(*it)->SetPositions(_fDeltaTick);
	}

	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++)
	{
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}

	for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++)
	{
		if ((*it)->IsActive())
			(*it)->SetPositions(_fDeltaTick);
	}
}


void GameScene::CheckCollisions()
{	
	// Enemy and player collisions
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{	
		for (auto it2 = m_enemies.begin(); it2 != m_enemies.end(); it2++)
		{
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.2f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.2f) &&  
				((*it)->GetEntityType() == PLAYER) && (*it2)->IsActive())
			{
				(*it)->SetToDead();
				(*it)->ReducePlayerLives();
				return;
			}
		}
	}

	// Enemy and enemy collisions
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++)
	{
		for (auto it2 = m_enemies.begin(); it2 != m_enemies.end(); it2++)
		{
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.2f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.2f) &&
				((*it)->IsActive()) && ((*it2)->IsActive()) &&
				(((*it)->GetModel()->GetPosition().x) != ((*it2)->GetModel()->GetPosition().x)) &&      // but not equal - that would be the same object
				(((*it)->GetModel()->GetPosition().z) != ((*it2)->GetModel()->GetPosition().z)))
			{
				(*it)->Flee((*it2)->GetModel()->GetPosition()); // Flee if enemies are too close together
			}
		}
	}


	// Player bullet and enemy collisions
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++)
	{
		for (auto it2 = m_playerBullets.begin(); it2 != m_playerBullets.end(); it2++)
		{
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.0f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.0f) &&
				((*it)->GetEntityType() == ENEMY) && ((*it2)->IsActive()) && ((*it)->IsActive()))
			{
				(*it)->GetModel()->SetToBeDeleted(); // set to delete enemy
				(*it2)->GetModel()->SetToBeDeleted(); // set to delete player bullet

				if ((*it)->GetModel()->IsLeader())
				{
					(*it)->SetLeaderDead();
				}

				(*it)->AddToScore(10);

				//break;
			}
		}
	}
}

bool GameScene::IsLevelComplete()
{
	// Check if level complete - no active enemies
	for (auto it = m_enemies.begin(); it != m_enemies.end(); it++)
	{
		if ((*it)->IsActive())
		{
			m_bLevelComplete = false;
			return m_bLevelComplete;
		}
	}

	m_bLevelComplete = true;
	return m_bLevelComplete;
}

void GameScene::NextLevel()
{
	m_iCurrentLevel++;
}

void GameScene::SetLevelComplete(bool _IsComplete)
{
	m_bLevelComplete = _IsComplete;
}

AIBehaviour GameScene::GetRandomBehaviour()
{
//  Not currently used

//	srand(time(NULL));
	int iBehaviour = rand() % 3;

	switch (iBehaviour)
	{
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

		}
		break;
	}
}

Entity* GameScene::GetPlayer()
{
	return m_pPlayer;
}

bool GameScene::GameOver()
{
	if (m_pPlayer->GetPlayerLives() < 0)
		return true;

	else
		return false;
}

void GameScene::SetLevel(int _iLevel)
{
	m_iCurrentLevel = _iLevel;
}

void GameScene::Update()
{
	GetClock()->Process();
	m_fDeltaTick = GetClock()->GetDeltaTick();
		
	if (GetPlayer()->IsPlayerDead())
	{
		SetPlayerAlive();  // Set player back to alive
		SetAllBulletsInactive(); // To stop bullet rendering

		if (GameOver())
		{
			SetLevel(1);  // go back to level one
			GetPlayer()->ResetPlayerLives(); 
			GetPlayer()->ResetScore();  
		}

		UpdateEntities(); // restart level		
	}

	else if (IsLevelComplete())
	{
		SetAllBulletsInactive(); // To stop bullet rendering
		NextLevel();
		UpdateEntities();   // start the next level
		SetLevelComplete(false);
	}

	CheckEnemies();  // Check if any enemies need to be deactivated - if set "to delete"
	CheckBullets();  // Check if any bullets need to be deactivated - if set "to delete"
	CheckCollisions();
	SetPositions(m_fDeltaTick);  // Set all entity positions	
}

void GameScene::SetUp()
{
	GLfloat width = 1600;
	GLfloat height = 900;

	Camera Camera(vec3(0.0f, 15.0f, 22.0f), vec3(0.0f, -0.9f, -1.0f), vec3(0.0f, 1.0f, 0.0f), width, height);
	//            Pos:  x      y      z    Front:       y tilt          Up:       

	AddCamera(Camera);
	CreateEntities();
	UpdateEntities();
	CreateText();
	GetClock()->Initialise();
}

void GameScene::SetAllBulletsInactive()
{
	for (auto it = m_playerBullets.begin(); it != m_playerBullets.end(); it++)
	{
		(*it)->SetActivity(false);
		//(*it)->ResetVelocityForBullets();		
	}
}

void GameScene::SetPlayerAlive()
{
	m_pPlayer->SetToAlive();
}

//void GameScene::SetDeltaTick(float _fTick)
//{
//	m_fDeltaTick = _fTick;
//}
//
//float GameScene::GetDeltaTick()
//{
//	return m_fDeltaTick;
//}


//void GameScene::ExecuteOneFrame()
//{
//	//float fDT = m_pClock->GetDeltaTick();
//
//	// Process(fDT);
//	// Draw();
//
//	//m_pClock->Process();
//
//	// Sleep(15);
//}

