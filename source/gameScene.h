//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : gameScene.h
// Description : Game scene header file
// Mail : Mitchell.Currie@mediadesignschool.com
//		  Juan.Rodriguez@mediadesignschool.com
//

#pragma once

//#include <Windows.h>
//#include <cassert>
//#include <thread>
//#include <ctime>
//
//
//#include "consoletools.h"
//#include "network.h"
//#include "client.h"
//#include "server.h"
//#include "InputLineBuffer.h"
//#include <functional>

#include "network.h"
#include "client.h"
#include "server.h"
#include <thread>
#include <mutex>

#include "camera.h"
#include "entity.h"
#include "clock.h"
#include "TextLabel.h"
#include "level.h"
#include <vector>
#include "FMOD\fmod.hpp"


//// make sure the winsock lib is included...
//#pragma comment(lib,"ws2_32.lib")

class GameScene {
public:

	GameScene();
	~GameScene();
	static GameScene& GetInstance();

	void AddCamera(Camera _camera);
	void AddEntity(Entity* _entity);
	void AddEnemy(Entity* _enemy);
	void AddPlayerBullet(Entity* _entity);
	void AddPlayer1Bullet(Entity* _entity);
	void AddPlayer2Bullet(Entity* _entity);
	void AddPlayer3Bullet(Entity* _entity);
	void AddPlayer4Bullet(Entity* _entity);
	void AddEnemyBullet(Entity* _entity);
	void AddPowerUp(Entity* _entity);
	void AddPlayer(Entity* _entity);
	void AddText(TextLabel* _text);

	void RenderEntities(); // For entities
	void RenderText(); // For text

	void CreateTextFields();
	void CreateEntities();
	void UpdateEntities();
	void CheckEnemyOnEnemyCollisions();
	void CheckPlayerOnEnemyCollisions();
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
//	AIBehaviour GetRandomBehaviour();
//	Entity* GetPlayer();
	Entity* GetPlayer1();
	Entity* GetPlayer2();
	Entity* GetPlayer3();
	Entity* GetPlayer4();
	bool arePlayersActive() const;
	float GetRandomPosition();

	// Levels
	bool IsLevelComplete();
	void NextLevel();
	void SetLevelComplete(bool _IsComplete);
	void SetLevel(int _iLevel);
	vec3 GetRandomMapPosition();
	std::string GetLives();
	int GetPlayerLives();
	void ReducePlayerLives();
	void ResetPlayerLives();
	void AddExtraLife();

	// Gameplay
	bool IsGameOver();
	void Update();
	void UpdateServer();
	void UpdateClient();
	void SetUp(int _fWidth, int _fHeight);
	bool AllPlayersDead();
	void SetPlayersToAlive();
	void AddToScore(int _Score);
	std::string GetScore();
	void ResetScore();
	void ResetScoreCounter();
	int GetScoreCounter();

	// Keyboard
	void GameScene::KeyDown(unsigned char key, int x, int y);
	void GameScene::KeyUp(unsigned char key, int x, int y);

	// Sounds
	bool InitFmod();
	const bool LoadAudio();

	// Networking
	void CreateNetwork();
	EEntityType GetNetworkEntityType();
	void SetNetworkEntityType(EEntityType _type);
	void UpdateNetwork();

	/*void SetDeltaTick(float _fTick);
	float GetDeltaTick();*/
	//	void ExecuteOneFrame();

	void UpdateEnemiesForClient();
	void UpdatePowerUpsForClient();
	void UpdateBulletsForClient();

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

	static std::clock_t m_startClientSendingTimer;
	static double m_durationClientSendingTimer;

	static int m_iExtraEnemies;
	Camera m_Camera;

	// Vectors
	std::vector<TextLabel*> m_textLabels;
	std::vector<Entity*> m_entities, m_players, m_enemies, m_playerBullets, m_player1Bullets, m_player2Bullets, m_player3Bullets, m_player4Bullets, m_enemyBullets, m_powerUps;

	// Pointers
	Entity* m_pBullet;
	//Entity* m_pPlayer;
	Entity* m_pPlayer1;
	Entity* m_pPlayer2;
	Entity* m_pPlayer3;
	Entity* m_pPlayer4;
	Entity* m_pEnemy;
	Entity* m_pMap;
	Entity* m_pPowerUp;
	TextLabel* m_pTextLabel;

	int m_iCurrentLevel;
	bool m_bLevelComplete;
	float m_fDeltaTick;

	int m_iPlayerLives;

	int m_iGameState;

	int m_iWidth, m_iHeight;

	int m_iScore;
	int m_iScoreCounter;

	static unsigned char keyState[255];
	static int m_iCursorIndex;      // changed from m_iMenuIndex

	// Sounds
	FMOD::System *m_pAudioMgr;
 
	FMOD::Sound
		*m_pBGMusic, *m_pDieSound, *m_pHitSound, *m_pPowerUpSound, *m_pSelectSound, *m_pShootSound, *m_pScrollSound;

	FMOD::Channel
		*m_pBGChannel, *m_pDieChannel, *m_pHitChannel, *m_pPowerUpChannel, *m_pSelectChannel, *m_pScrollChannel, *m_pShootChannel;

	// Networking
	EEntityType m_eNetworkEntityType;
	CNetwork* m_pNetwork;

	std::thread m_ReceiveClientThread;
	std::thread m_ReceiveServerThread;

	CClient* m_pClient;
	CServer* m_pServer;

	static int m_iClientChoices;

	char* m_pcPacketData;

	bool bCanCheckConnectedUsers = false;

	// Network timers
	std::clock_t startTimeKA;
	std::clock_t currentTimeKA;
	double deltaTimeKA;

	// Keep alive check
	std::clock_t startTimeKAC;
	std::clock_t currentTimeKAC;
	double deltaTimeKAC;

	TPacket m_packet;
	std::string username;
	char* usernameC;

	bool m_bUpdateEntities;

//	std::mutex EnemiesMutex;

};