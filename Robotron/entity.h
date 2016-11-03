//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : entity.h
// Description : Entity header file - sets indices and vertices, VBO, VAO, EBO, performs rotations and updates model position
// Mail : Mitchell.Currie@mediadesignschool.com
//

#pragma once

#include "model.h"
#include <ctime>

class Entity
{
public:

	Entity();
	~Entity();
	virtual void Initialise(EntityType _entity, ModelType _model, GLsizei _numVertices, Camera _camera, vec3 _position, AIBehaviour _behaviour, float _maxVelocity, char* _filename);
	virtual void Render();
	virtual void SetPositions(float _fDeltaTick);
	
	static void entityKeyboard(unsigned char key, int x, int y);
	static void entityKeyboard_up(unsigned char key, int x, int y);
	static void IncrementPlayerBulletCounter();
	static void IncrementEnemyBulletCounter();
	static void IncrementEnemyCounter();
	static int GetPlayerBulletCounter();
	static int GetEnemyBulletCounter();
	static int GetEnemyCounter();
	static void ResetEnemyCounter();

	// AI functions
	void Seek(vec3 _playerPosition);
	void Flee(vec3 _playerPosition);
	void Pursue();
	void Evade();
	void EvadeLeader();
	void Wander();
	void LeaderFollowing();
	void Flocking();
	void SetAIBehaviour(AIBehaviour _behaviour);
	AIBehaviour GetAIBehaviour();
	
	// Forces and velocity
	vec3 LimitForce(vec3 _steering);
	vec3 LimitVelocity(vec3 _velocity);
	vec3 GetPlayerVelocity();
	vec3 GetCurrentVelocity();
	void ReverseCurrentVelocity();
	void ResetCurrentVelocity();
	void SetMaxVelocity(float _fMaxVelocity);
	void ResetVelocityForPlayerBullets();
	void VelocityToZero();
	void ResetMaxVelocity();
	void SpeedUp();
	void SetSpedUp(bool _b);
	void FastFire();
	void SetFastFire(bool _b);
	void ResetFireRate();
	void SetEnemyToFlee(bool _b);

	static bool IsBulletFired();
	bool ToDelete();
	bool IsPlayer();
	bool IsActive();
	bool IsLeader();
	bool IsPlayerDead();
	bool IsSpedUp();
	bool IsFiringFast();
	bool IsFleeing();
	//bool IsEnemyFiring();

	std::string GetAIName();
	vec2 GetTextPosition();
	std::string GetScore();
	std::string GetLives();
	Model* GetModel();
	EntityType GetEntityType();
	int GetPlayerLives();
	PowerUpType GetPowerUpType();
	
	void SetLeader(bool _b);
	void SetAsPlayer();	
	void SetToDead();
	void SetLeaderDead();
	void SetActivity(bool _activity);
	void SetModelPosition(vec3 _position);
	void SetBulletFired(bool _b);
	void SetToAlive();
	void SetAsEnemyBullet();
	void SetModelOutsideMap(bool _b);
	void SetPowerUpType(PowerUpType _type);
	
	void AddToScore(int _Score);	
	void ReducePlayerLives();
	void ResetPlayerLives();
	void ResetScore();		
	void AddExtraLife();
	void ResetScoreCounter();
	int GetScoreCounter();
	
private:

	static std::clock_t m_start;
	static double m_duration;
	static vec3 m_CurrentPlayerVelocity;
	static vec3 m_CurrentLeaderVelocity;
	static vec3 m_LastBulletVelocity;
	static int m_iPlayerBulletCounter;
	static int m_iEnemyBulletCounter;
	static int m_iEnemyCounter;
	static bool m_bLeaderDead;
	static bool m_bBulletFired;
	
	Model* m_pModel;

	vec3 m_CurrentVelocity;
	vec3 m_CurrentBulletVelocity;
	EntityType m_EntityType;
	std::string m_AIName;
	AIBehaviour m_Behaviour;
	vec2 m_textPosition;
	PowerUpType m_PowerUp;
	
	float m_fMaxForce;
	float m_fMaxVelocity;
	float m_fStartingMaxVelocity;
	float m_fFireRate;
	int m_iScore;
	int m_iScoreCounter;
	int m_iPlayerLives;
					
	bool m_bSetBulletDirection;
	bool m_bIsLeader;
	bool m_bIsPlayer;
	bool m_bIsDead;
	bool m_bActive;	
	bool m_bIsEnemyBullet;
	bool m_bIsSpedUp;
	bool m_bFastFire;
	bool m_bIsFleeing;
//	bool m_bEnemyToFire;
};

