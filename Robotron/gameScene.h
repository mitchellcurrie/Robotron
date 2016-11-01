//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : gameScene.h
// Description : Game scene header file - singleton game scene class that creates the models
// Mail : Mitchell.Currie@mediadesignschool.com
//

#pragma once

#include "camera.h"
#include "entity.h"
#include "clock.h"
#include "TextLabel.h"
#include "level.h"
#include <vector>

class GameScene
{
public:

	GameScene();
	~GameScene();
	static GameScene& GetInstance();

	void AddCamera(Camera _camera);
	void AddEntity(Entity* _entity);
	void AddEnemy(Entity* _enemy);
	void AddPlayerBullet(Entity* _entity);
	void AddText(TextLabel* _text);

	void RenderEntities(); // For entities
	void RenderText(); // For text
	
	void CreateText();
	void CreateEntities();
	void UpdateEntities();
	void CheckCollisions();
	void CheckBullets();
	void CheckEnemies();
	void SetAllBulletsInactive();
	void SetPositions(float _fDeltaTick);
	void SetPlayerAlive();

	CClock* GetClock();
	AIBehaviour GetRandomBehaviour();
	Entity* GetPlayer();

	// Levels
	bool IsLevelComplete();
	void NextLevel();
	void SetLevelComplete(bool _IsComplete);
	void SetLevel(int _iLevel);

	// Gameplay
	bool GameOver();
	void Update();
	void SetUp();
	
	/*void SetDeltaTick(float _fTick);
	float GetDeltaTick();*/
	//	void ExecuteOneFrame();
	
protected:
	CClock* m_pClock;

private:

	static GameScene* s_pGameScene;
	Camera m_Camera;

	// Vectors
	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_enemies;
	std::vector<TextLabel*> m_textLabels;
	std::vector<Entity*> m_playerBullets;

	// Pointers
	Entity* m_pBullet;
	Entity* m_pPlayer;
	Entity* m_pEnemy;
	Entity* m_pMap;
	TextLabel* m_pTextLabel;

	int m_iCurrentLevel;
	bool m_bLevelComplete;
	float m_fDeltaTick;


};
