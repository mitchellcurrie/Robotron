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
	virtual void Initialise(EntityType _entity, ModelType _model, GLsizei _numVertices, Camera _camera, vec3 _position, AIBehaviour _behaviour, float _maxVelocity);
	virtual void Render();
	virtual void SetPositions(float _fDeltaTick);
	
	static void entityKeyboard(unsigned char key, int x, int y);
	static void entityKeyboard_up(unsigned char key, int x, int y);
	static void IncrementPlayerBulletCounter();
	static void IncrementEnemyBulletCounter();
	static int GetPlayerBulletCounter();
	static int GetEnemyBulletCounter();

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
	void ResetVelocity();

	static bool IsBulletFired();
	bool ToDelete();
	bool IsPlayer();
	bool IsActive();
	bool IsLeader();
	bool IsPlayerDead();
	//bool IsEnemyFiring();

	std::string GetAIName();
	vec2 GetTextPosition();
	std::string GetScore();
	std::string GetLives();
	Model* GetModel();
	EntityType GetEntityType();
	int GetPlayerLives();
	
	void SetLeader(bool _b);
	void SetAsPlayer();	
	void SetToDead();
	void SetLeaderDead();
	void SetActivity(bool _activity);
	void SetModelPosition(vec3 _position);
	void SetBulletFired(bool _b);
	void SetToAlive();
	void SetAsEnemyBullet();
	
	void AddToScore(int _Score);	
	void ReducePlayerLives();
	void ResetPlayerLives();
	void ResetScore();	
	
	
private:

	static std::clock_t m_start;
	static double m_duration;
	static vec3 m_CurrentPlayerVelocity;
	static vec3 m_CurrentLeaderVelocity;
	static vec3 m_LastBulletVelocity;
	static int m_iPlayerLives;
	static int m_iPlayerBulletCounter;
	static int m_iEnemyBulletCounter;
	static int m_iScore;
	static bool m_bLeaderDead;
	static bool m_bBulletFired;
	
	Model* m_pModel;

	vec3 m_CurrentVelocity;
	vec3 m_CurrentBulletVelocity;
	EntityType m_EntityType;
	std::string m_AIName;
	AIBehaviour m_Behaviour;
	vec2 m_textPosition;
	
	float m_fMaxForce;
	float m_fMaxVelocity;
					
	bool m_bSetBulletDirection;
	bool m_bIsLeader;
	bool m_bIsPlayer;
	bool m_bIsDead;
	bool m_bActive;	
	bool m_bIsEnemyBullet;
//	bool m_bEnemyToFire;
};

