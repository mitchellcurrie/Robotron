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
	virtual void Initialise(EntityType _entity, ModelType _model, GLsizei _numVertices, Camera _camera, vec3 _position, bool _IsPlayer, bool _IsLeader);
	virtual void Render();
	bool IsPlayer();
	static void entityKeyboard(unsigned char key, int x, int y);
	static void entityKeyboard_up(unsigned char key, int x, int y);
	vec3 LimitForce(vec3 _steering);
	vec3 LimitVelocity(vec3 _velocity);
	void Seek(vec3 _playerPosition);
	void Flee(vec3 _playerPosition);
	void Pursue();
	void Evade();
	void Wander();
	void LeaderFollowing();
	void Flocking();
	Model* GetModel();
	virtual void SetPositions(float _fDeltaTick);
	void ReverseCurrentVelocity();
	void ResetCurrentVelocity();
	vec3 GetPlayerVelocity();
	vec3 GetCurrentVelocity();
	EntityType GetEntityType();
	static bool IsBulletFired();
	bool ToDelete();
	std::string GetAIName();
	vec2 GetTextPosition();
	void SetLeaderDead();
	
private:

	Model* m_pModel;
	vec3 m_CurrentVelocity;
	static vec3 m_CurrentPlayerVelocity;
	static vec3 m_CurrentLeaderVelocity;
	float m_fMaxForce;
	float m_fMaxVelocity;
	EntityType m_EntityType;
	static bool m_bBulletFired;
	bool SetBulletDirection;
	static std::clock_t m_start;
	static double m_duration;
	static std::string m_AIName;
	static vec2 m_textPosition;
	static bool m_bLeaderDead;
};

