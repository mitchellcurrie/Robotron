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
	void AddEnemyBullet(Entity* _entity);
	void AddPowerUp(Entity* _entity);
	void AddText(TextLabel* _text);

	void RenderEntities(); // For entities
	void RenderText(); // For text
	
	void CreateTextFields();
	void CreateEntities();
	void UpdateEntities();
	void CheckCollisions();
	void CheckBullets();
	void CheckEnemies();
	void CheckPowerUps();
	void SetAllBulletsInactive();
	void SetPositions(float _fDeltaTick);
	void SetPlayerAlive();
	bool AllPowerUpsActive();
	void SetEnemiesToFlee();
	void ResetEnemyBehaviours();

	CClock* GetClock();
	AIBehaviour GetRandomBehaviour();
	Entity* GetPlayer();
	float GetRandomPosition();

	// Levels
	bool IsLevelComplete();
	void NextLevel();
	void SetLevelComplete(bool _IsComplete);
	void SetLevel(int _iLevel);
	vec3 GetRandomMapPosition();

	// Gameplay
	bool IsGameOver();
	void Update();
	void SetUp(int _fWidth, int _fHeight);

	// Keyboard
	void GameScene::KeyDown(unsigned char key, int x, int y);

	void GameScene::KeyUp(unsigned char key, int x, int y);
	
	/*void SetDeltaTick(float _fTick);
	float GetDeltaTick();*/
	//	void ExecuteOneFrame();
	
protected:
	CClock* m_pClock;

private:

	static GameScene* s_pGameScene;

	//Timers
	static std::clock_t m_startAITimer;
	static double m_durationAITimer;

	static std::clock_t m_startSpeedBoostTimer;
	static double m_durationSpeedBoostTimer;

	static std::clock_t m_startFastFireTimer;
	static double m_durationFastFireTimer;

	static std::clock_t m_startEnemyFleeTimer;
	static double m_durationEnemyFleeTimer;

	static int m_iExtraEnemies;
	Camera m_Camera;

	// Vectors
	std::vector<TextLabel*> m_textLabels;
	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_enemies;
	std::vector<Entity*> m_playerBullets;
	std::vector<Entity*> m_enemyBullets;
	std::vector<Entity*> m_powerUps;

	// Pointers
	Entity* m_pBullet;
	Entity* m_pPlayer;
	Entity* m_pEnemy;
	Entity* m_pMap;
	Entity* m_pPowerUp;
	TextLabel* m_pTextLabel;

	int m_iCurrentLevel;
	bool m_bLevelComplete;
	float m_fDeltaTick;

	int m_iGameState;

	int m_iWidth, m_iHeight;

	static unsigned char keyState[255];
	static int m_iMenuIndex;

};