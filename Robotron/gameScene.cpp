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

void GameScene::AddBullet(Entity* _entity)
{
	m_bullets.push_back(_entity);
}

void GameScene::AddText(TextLabel* _text)
{
	m_textLabels.push_back(_text);
}

void GameScene::CheckBullets()
{	
	std::vector<Entity*> bulletsToKeep;

	if (Entity::IsBulletFired())
	{
		m_pBullet = new Entity;
		m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, m_pPlayer->GetModel()->GetPlayerPosition(), false, false, NONENEMY, 0.0f);
		AddBullet(m_pBullet);
	}

	for (auto it = m_bullets.begin(); it != m_bullets.end(); it++)
	{
		if (!(*it)->ToDelete())
		{
			bulletsToKeep.push_back(*it);
		}
	}

	while (!m_bullets.empty())
	{
		DeleteEntity(m_bullets.back());
		m_bullets.pop_back();
	}

	m_bullets = bulletsToKeep;
}

void GameScene::CheckEntities()
{
	std::vector<Entity*> entitiesToKeep;

	//if (Entity::IsBulletFired())    // Change this to be able to add new enemies
	//{
	//	m_pBullet = new Entity;
	//	m_pBullet->Initialise(BULLET, DOT, 6, m_Camera, m_pPlayer->GetModel()->GetPlayerPosition(), false, false);
	//	AddBullet(m_pBullet);
	//}

	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{
		if (!((*it))->ToDelete())
		{
			entitiesToKeep.push_back(*it);
		}
	}

	while (!m_entities.empty())
	{
		DeleteEntity(m_entities.back());
		m_entities.pop_back();
	}

	m_entities = entitiesToKeep;
}

void GameScene::DeleteEntity(Entity* _entity)
{
	/*delete _bullet;
	_bullet = nullptr;*/

	// Need another way to delete the bullet memory, at the moment just popping from vector without deleting first
}

void GameScene::DeleteEntity(Entity _entity)
{
	/*delete _bullet;
	_bullet = nullptr;*/

	// Need another way to delete the entity memory, at the moment just popping from vector without deleting first
}

void GameScene::RenderEntities()
{
	for (auto it = m_bullets.begin(); it != m_bullets.end(); it++)
	{
		if ((*it)->GetModel() != nullptr)
			(*it)->Render();
	}

	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{		
		(*it)->Render();	
	}
}

void GameScene::RenderText()
{
	for (auto it = m_textLabels.begin(); it != m_textLabels.end(); it++)
	{
		if ((*it)->GetTextType() == AIDESCRIPTION)
		{
			if (m_iCurrentLevel < 8)
			{
				(*it)->setText((*m_entities.begin())->GetAIName());
				(*it)->setPosition((*m_entities.begin())->GetTextPosition());
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
		
		(*it)->Render();
	}
}

void GameScene::CreateEntities()
{
	m_bullets.clear();  // Need to clear the memoery of the objects here, not just clear the vectors!!
	m_entities.clear();  // Need to clear the memoery of the objects here, not just clear the vectors!!
						// Also on collisions, need to delete memory not just clear vectors!

	
	// Player Cube
	m_pPlayer = new Entity;
	m_pPlayer->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(-12.0f, 0.0f, 12.0f), true, false, NONENEMY, 0.0f);
	AddEntity(m_pPlayer);

	// Map Quad
	m_pMap = new Entity;
	m_pMap->Initialise(MAP, QUAD, 6, m_Camera, vec3(0.0f, 0.0f, 0.0f), false, false, NONENEMY, 0.0f);
	AddEntity(m_pMap);

	if (m_iCurrentLevel == 1)
	{
		// Enemy Cubes
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), false, true, FLEE, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), false, false, FLEE, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), false, false, FLEE, 0.05f);
		AddEntity(m_pEnemy);
	}

	else if (m_iCurrentLevel == 2)
	{
		// Enemy Cubes
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), false, true, EVADE, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), false, false, EVADE, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), false, false, EVADE, 0.05f);
		AddEntity(m_pEnemy);
	}

	else if (m_iCurrentLevel == 3)
	{
		// Enemy Cubes
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), false, true, WANDER, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), false, false, WANDER, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), false, false, WANDER, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), false, false, WANDER, 0.05f);
		AddEntity(m_pEnemy);
	}

	else if (m_iCurrentLevel == 4)
	{
		// Enemy Cubes
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), false, true, SEEK, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), false, false, SEEK, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), false, false, SEEK, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), false, false, SEEK, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(1.0f, 0.0f, 1.0f), false, false, SEEK, 0.05f);
		AddEntity(m_pEnemy);
	}

	else if (m_iCurrentLevel == 5)
	{
		// Enemy Cubes
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), false, true, LEADERFOLLOW, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), false, false, LEADERFOLLOW, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), false, false, LEADERFOLLOW, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), false, false, LEADERFOLLOW, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(1.0f, 0.0f, 1.0f), false, false, LEADERFOLLOW, 0.05f);
		AddEntity(m_pEnemy);
	}

	else if (m_iCurrentLevel == 6)
	{
		// Enemy Cubes
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), false, true, FLOCK, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), false, false, FLOCK, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), false, false, FLOCK, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), false, false, FLOCK, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(1.0f, 0.0f, 1.0f), false, false, FLOCK, 0.05f);
		AddEntity(m_pEnemy);
	}

	else if (m_iCurrentLevel == 7)
	{
		// Enemy Cubes
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), false, true, PURSUE, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), false, false, PURSUE, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), false, false, PURSUE, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), false, false, PURSUE, 0.05f);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(1.0f, 0.0f, 1.0f), false, false, PURSUE, 0.05f);
		AddEntity(m_pEnemy);
	}

	else // After level 7
	{	
		float fMaxVelocity = 0.05f + ((static_cast<float>(m_iCurrentLevel)) / 350.0f);
	//	float fMaxVelocity = 0.0001f;  // To test positions
	//	float fMaxVelocity = 0.05f;
	
		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 0.0f), false, true, SEEK, fMaxVelocity); //GetRandomBehaviour()
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, 5.0f), false, true, SEEK, fMaxVelocity); //GetRandomBehaviour()
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-2.0f, 0.0f, 2.0f), false, true, SEEK, fMaxVelocity); //GetRandomBehaviour()
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(5.0f, 0.0f, -5.0f), false, false, PURSUE, fMaxVelocity);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(2.0f, 0.0f, -2.0f), false, false, PURSUE, fMaxVelocity); 
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, -5.0f), false, false, PURSUE, fMaxVelocity);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(0.0f, 0.0f, 0.0f), false, false, PURSUE, fMaxVelocity);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-2.0f, 0.0f, -2.0f), false, false, WANDER, fMaxVelocity);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(2.0f, 0.0f, 2.0f), false, false, WANDER, fMaxVelocity);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(-5.0f, 0.0f, 5.0f), false, false, WANDER, fMaxVelocity);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(10.0f, 0.0f, 0.0f), false, false, WANDER, fMaxVelocity);
		AddEntity(m_pEnemy);

		m_pEnemy = new Entity;
		m_pEnemy->Initialise(ENEMY, CUBE, 36, m_Camera, vec3(10.0f, 0.0f, 0.0f), false, false, WANDER, fMaxVelocity);
		AddEntity(m_pEnemy);
	}



	// Dot - small quad for bullet
	/*Entity Bullet;
	Bullet.Initialise(BULLET, DOT, 6, m_Camera, vec3(1.0f, 0.0f, 1.0f), false, false);
	AddEntity(Bullet);*/

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

	for (auto it = m_bullets.begin(); it != m_bullets.end(); it++)
	{
		(*it)->SetPositions(_fDeltaTick);
	}
}


void GameScene::CheckCollisions()
{	
	// CUBE collisions - enemys and players
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{	
		for (auto it2 = m_entities.begin(); it2 != m_entities.end(); it2++)
		{
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.2f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.2f) &&   // within a distance
				(((*it)->GetModel()->GetPosition().x) != ((*it2)->GetModel()->GetPosition().x)) &&      // but not equal - that would be the same object
				(((*it)->GetModel()->GetPosition().z) != ((*it2)->GetModel()->GetPosition().z)) &&
				((*it)->GetModel()->GetModelType() == CUBE) && // both objects CUBES - so it doesn't include the map and other entities
				((*it2)->GetModel()->GetModelType() == CUBE))
			{
				if ((((*it)->GetEntityType() == PLAYER) && ((*it2)->GetEntityType() == ENEMY)) ||
					(((*it)->GetEntityType() == ENEMY) && ((*it2)->GetEntityType() == PLAYER)))

				{
					// Player dies
				}
					
				else if (((*it)->GetEntityType() == ENEMY) && ((*it2)->GetEntityType() == ENEMY))
				{
					(*it)->Flee((*it2)->GetModel()->GetPosition());
				//	break;
				}
			
			}

		}
	}

	// BULLET and ENEMY collisions
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{
		for (auto it2 = m_bullets.begin(); it2 != m_bullets.end(); it2++)
		{
			if ((abs(((*it)->GetModel()->GetPosition().x) - ((*it2)->GetModel()->GetPosition().x)) < 1.5f) &&   // within a distance
				(abs(((*it)->GetModel()->GetPosition().z) - ((*it2)->GetModel()->GetPosition().z)) < 1.5f) &&
				((*it)->GetEntityType() == ENEMY))
			{
				(*it)->GetModel()->SetToBeDeleted(); // delete enemy
				(*it2)->GetModel()->SetToBeDeleted(); // delete bullet

				if ((*it)->GetModel()->IsLeader())
				{
					(*it)->SetLeaderDead();
				}

				(*it)->AddToScore(10);

				break;
			}

			//if ((*it2)->GetCurrentVelocity() == vec3(0.0f, 0.0f, 0.0f))
			//{
			//	(*it2)->GetModel()->SetBulletToBeDeleted();                // to try to sop flashing bullet on the player spawn point - not working
			//}
		}

	}
}

//void GameScene::CreateLevels()
//{
//	// Level 1
//	m_pLevel = new Level;
//	m_pLevel->Initialise(1, 3.0f, FLEE);
//	AddLevel(m_pLevel);
//
//	// Level 2
//	m_pLevel = new Level;
//	m_pLevel->Initialise(5, 5.0f, SEEK);
//	AddLevel(m_pLevel);
//}

//void GameScene::AddLevel(Level* _level)
//{
//	m_levels.push_back(_level);
//}

bool GameScene::IsLevelComplete()
{
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{
		if ((*it)->GetEntityType() == ENEMY)
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

