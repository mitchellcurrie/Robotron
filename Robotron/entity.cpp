//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : Entity.cpp
// Description : Entity implementation file 
// Mail : Mitchell.Currie@mediadesignschool.com
//

#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <math.h>
#include <time.h>  
#include <string>

#include "glew.h"
#include "freeglut.h"
#include "entity.h"
#include "ShaderLoader.h"
#include "utils.h"

unsigned char keyState[255];

// Static variables
std::clock_t Entity::m_start = std::clock();
double Entity::m_duration = 0.0f;

vec3 Entity::m_CurrentPlayerVelocity = vec3(0.0f, 0.0f, 0.0f);
vec3 Entity::m_CurrentLeaderVelocity = vec3(0.0f, 0.0f, 0.0f);
vec3 Entity::m_LastBulletVelocity = vec3(0.0f, 0.0f, -0.94167f); // -0.94 - In case the player shoots before using the shooting direction controls, shoots forward

bool Entity::m_bBulletFired = false;
bool Entity::m_bLeaderDead = false;

int Entity::m_iScore = 0;
int Entity::m_iPlayerLives = 3;
int Entity::m_iBulletCounter = 0;


Entity::Entity()
{
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);

	// To update:
	m_fMaxForce = 0.05f;
	m_fMaxVelocity = 0.00f;
	m_bSetBulletDirection = false;
	m_bIsLeader = false;
	m_bIsPlayer = false;
	m_bIsDead = false;
	m_bActive = false;
	std::string m_AIName = "";
	vec2 m_textPosition = vec2(0.0f, 0.0f);
	srand(time(NULL));
}

Entity::~Entity()
{

}

void Entity::Initialise(EntityType _entity, ModelType _model, GLsizei _numVertices, Camera _camera, vec3 _position, AIBehaviour _behaviour, float _maxVelocity)
{
	m_pModel = new Model;
	m_EntityType = _entity;
	m_pModel->Initialise(_model, _numVertices, _camera, _position, IsPlayer(), IsLeader());	
	m_Behaviour = _behaviour;
	m_fMaxVelocity = _maxVelocity;

	if (m_EntityType == PLAYER)
		m_start = std::clock();
}


void Entity::Render()
{
	m_pModel->Render(m_CurrentVelocity);
}

void Entity::SetPositions(float _fDeltaTick)
{
	if (m_EntityType == PLAYER)
	{
		m_bBulletFired = false;

		////////////  PLAYER Direction ////////////////

		float fSpeed = 8;

		// Forward left Movement
		if ((keyState[(unsigned char)'a'] == BUTTON_DOWN) && (keyState[(unsigned char)'w'] == BUTTON_DOWN))
		{
			printf("Move cube forward left \n");
			m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Forward right Movement
		else if ((keyState[(unsigned char)'d'] == BUTTON_DOWN) && (keyState[(unsigned char)'w'] == BUTTON_DOWN))
		{
			printf("Move cube forward right \n");
			m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Backward left Movement
		else if ((keyState[(unsigned char)'a'] == BUTTON_DOWN) && (keyState[(unsigned char)'s'] == BUTTON_DOWN))
		{
			printf("Move cube backward left \n");
			m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Backward right Movement
		else if ((keyState[(unsigned char)'d'] == BUTTON_DOWN) && (keyState[(unsigned char)'s'] == BUTTON_DOWN))
		{
			printf("Move cube backward right \n");
			m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Left Movement
		else if (keyState[(unsigned char)'a'] == BUTTON_DOWN)
		{
			printf("Move cube left \n");
			m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, 0.0f) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Right movement
		else if (keyState[(unsigned char)'d'] == BUTTON_DOWN)
		{
			printf("Move cube right \n");
			m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, 0.0f) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Forward movement
		else if (keyState[(unsigned char)'w'] == BUTTON_DOWN)
		{
			printf("Move cube forward \n");
			m_CurrentVelocity = glm::vec3(0.0f, 0.0f, -fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Back movement
		else if (keyState[(unsigned char)'s'] == BUTTON_DOWN)
		{
			printf("Move cube back \n");
			m_CurrentVelocity = glm::vec3(0.0f, 0.0f, fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		else
		{
			m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
		}

		////////////  BULLET Firing ////////////////

		if (keyState[(unsigned char)32] == BUTTON_DOWN) // space bar
		{
			m_duration = (std::clock() - m_start) / (double)CLOCKS_PER_SEC;
			
			if (m_duration > 0.17f)
			{
				printf("Fire \n");
				m_bBulletFired = true;
				m_start = std::clock();
			}		
		}

		m_CurrentPlayerVelocity = m_CurrentVelocity;
	}

	else if (m_EntityType == ENEMY)// Enemy movement
	{	
		switch (m_Behaviour)
		{
			case SEEK:
			{
				Seek(m_pModel->GetPlayerPosition());
				m_AIName = "Seek";
				m_textPosition = vec2(720, 860);
			}
			break;

			case FLEE:
			{
				Flee(m_pModel->GetPlayerPosition());
				m_AIName = "Flee";
				m_textPosition = vec2(720, 860);
			}
			break;

			case PURSUE:
			{
				Pursue();
				m_AIName = "Pursue";
				m_textPosition = vec2(670, 860);
			}
			break;

			case EVADE:
			{
				Evade();
				m_AIName = "Evade";
				m_textPosition = vec2(690, 860);
			}
			break;

			case WANDER:
			{
				Wander();
				m_AIName = "Wander";
				m_textPosition = vec2(680, 860);
			}
			break;

			case LEADERFOLLOW:
			{
				LeaderFollowing();
				m_AIName = "Leader Following";
				m_textPosition = vec2(495, 860);
			}
			break;

			case FLOCK:
			{
				Flocking();
				m_AIName = "Flocking";
				m_textPosition = vec2(658, 860);
			}
			break;

			default:
			{

			}
			break;
		}

		if (m_pModel->IsLeader())
			m_CurrentLeaderVelocity = m_CurrentVelocity;
	}

	else if ((m_EntityType == BULLET) && (m_bActive))
	{
		////////////  BULLET Direction ////////////////

		//if (m_pModel->IsAtEdge())
		//{
		//	m_pModel->SetToBeDeleted(); // m_bToBeDeleted = true;
		//}

		float fSpeed = 45;

		// Forward left 
		if ((keyState[(unsigned char)'j'] == BUTTON_DOWN) && (keyState[(unsigned char)'i'] == BUTTON_DOWN))
		{
			//printf("Move cube forward left \n");
			m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
			//m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
			m_LastBulletVelocity = m_CurrentBulletVelocity;
		}

		// Forward right 
		else if ((keyState[(unsigned char)'l'] == BUTTON_DOWN) && (keyState[(unsigned char)'i'] == BUTTON_DOWN))
		{
			//printf("Move cube forward right \n");
			m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
			//m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
			m_LastBulletVelocity = m_CurrentBulletVelocity;
		}

		// Backward left 
		else if ((keyState[(unsigned char)'j'] == BUTTON_DOWN) && (keyState[(unsigned char)'k'] == BUTTON_DOWN))
		{
			//printf("Move cube backward left \n");
			m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, fSpeed) * _fDeltaTick;
			//m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
			m_LastBulletVelocity = m_CurrentBulletVelocity;
		}

		// Backward right 
		else if ((keyState[(unsigned char)'k'] == BUTTON_DOWN) && (keyState[(unsigned char)'l'] == BUTTON_DOWN))
		{
			//printf("Move cube backward right \n");
			m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, fSpeed) * _fDeltaTick;
			//m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
			m_LastBulletVelocity = m_CurrentBulletVelocity;
		}

		// Left 
		else if (keyState[(unsigned char)'j'] == BUTTON_DOWN)
		{
			//printf("Move cube left \n");
			m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, 0.0f) * _fDeltaTick;
			//m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
			m_LastBulletVelocity = m_CurrentBulletVelocity;
		}

		// Right 
		else if (keyState[(unsigned char)'l'] == BUTTON_DOWN)
		{
			//printf("Move cube right \n");
			m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, 0.0f) * _fDeltaTick;
			//m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
			m_LastBulletVelocity = m_CurrentBulletVelocity;
		}

		// Forward 
		else if (keyState[(unsigned char)'i'] == BUTTON_DOWN)
		{
			//printf("Move cube forward \n");
			m_CurrentBulletVelocity = glm::vec3(0.0f, 0.0f, -fSpeed) * _fDeltaTick;
			//m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
			m_LastBulletVelocity = m_CurrentBulletVelocity;
		}

		// Back 
		else if (keyState[(unsigned char)'k'] == BUTTON_DOWN)
		{
			//printf("Move cube back \n");
			m_CurrentBulletVelocity = glm::vec3(0.0f, 0.0f, fSpeed) * _fDeltaTick;
			//m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
			m_LastBulletVelocity = m_CurrentBulletVelocity;
		}

		if (!m_bSetBulletDirection)
		{
			// m_CurrentVelocity = m_pModel->GetLastPlayerVelocity() * 7.0f;
			m_CurrentVelocity = m_LastBulletVelocity;
			m_bSetBulletDirection = true;
		}
			
	}
}

bool Entity::IsPlayer()
{
	if (m_EntityType == PLAYER)
		return true;

	else
		return false;
}

void Entity::entityKeyboard(unsigned char key, int x, int y)
{
	keyState[key] = BUTTON_DOWN;
}

void Entity::entityKeyboard_up(unsigned char key, int x, int y)
{
	keyState[key] = BUTTON_UP;
}

vec3 Entity::LimitForce(vec3 _steering)
{
	vec3 Steering = _steering;
	
	if (_steering.x > m_fMaxForce)
		Steering.x = m_fMaxForce;

	if (_steering.y > m_fMaxForce)
		Steering.y = m_fMaxForce;

	if (_steering.z > m_fMaxForce)
		Steering.z = m_fMaxForce;

	return Steering;
}

vec3 Entity::LimitVelocity(vec3 _velocity)
{
	vec3 Velocity = _velocity;

	if (_velocity.x > m_fMaxVelocity)
		Velocity.x = m_fMaxVelocity;

	if (_velocity.y > m_fMaxVelocity)
		Velocity.y = m_fMaxVelocity;

	if (_velocity.z > m_fMaxVelocity)
		Velocity.z = m_fMaxVelocity;

	return Velocity;
}

void Entity::Seek(vec3 _playerPosition)
{
	vec3 DesiredVelocity = _playerPosition - m_pModel->GetPosition();
	DesiredVelocity = glm::normalize(DesiredVelocity);

	DesiredVelocity *= m_fMaxVelocity;

	vec3 SteeringForce = DesiredVelocity - m_CurrentVelocity;
	SteeringForce = LimitForce(SteeringForce);

	m_CurrentVelocity += SteeringForce;
	m_CurrentVelocity = LimitVelocity(m_CurrentVelocity);
}

void Entity::Flee(vec3 _playerPosition)
{
	vec3 DesiredVelocity = m_pModel->GetPosition() - _playerPosition;
	DesiredVelocity = glm::normalize(DesiredVelocity);

	DesiredVelocity *= m_fMaxVelocity;

	vec3 SteeringForce = DesiredVelocity - m_CurrentVelocity;
	SteeringForce = LimitForce(SteeringForce);

	m_CurrentVelocity += SteeringForce;
	m_CurrentVelocity = LimitVelocity(m_CurrentVelocity);
}

void Entity::Pursue()
{
	vec3 Distance = m_pModel->GetPlayerPosition() - m_pModel->GetPosition();

	//float T = Distance.length() / (m_fMaxVelocity + GetPlayerVelocity().length());
	float T = Distance.length() / m_fMaxVelocity;
	//float T = length(Distance) / m_fMaxVelocity;
	// float T = 50.0f;

	vec3 FuturePosition = m_pModel->GetPlayerPosition() + (m_CurrentPlayerVelocity * T);

	if (m_pModel->IsWithinPlayerRange(3.0f))
		Seek(m_pModel->GetPlayerPosition());

	else
		Seek(FuturePosition);

}

void Entity::Evade()
{
	vec3 Distance = m_pModel->GetPlayerPosition() - m_pModel->GetPosition();

	float T = Distance.length() / m_fMaxVelocity;
	//float T = 50.0f;

	vec3 FuturePosition = m_pModel->GetPlayerPosition() + m_CurrentPlayerVelocity * T;

	if (m_pModel->IsWithinPlayerRange(6.0f))
		Flee(m_pModel->GetPlayerPosition());

	else
		Flee(FuturePosition);

}

void Entity::EvadeLeader()
{
	vec3 Distance = m_pModel->GetLeaderPosition() - m_pModel->GetPosition();

	float T = Distance.length() / m_fMaxVelocity;
	//float T = 50.0f;

	vec3 FuturePosition = m_pModel->GetLeaderPosition() + m_CurrentLeaderVelocity * T;

	Flee(FuturePosition);
}

void Entity::Wander()
{
	float Distance = 1500.0f;
	float Radius = 7.0f;
	vec3 TargetPosition = vec3(0.0f, 0.0f, 0.0f);

	vec3 FuturePosition = m_pModel->GetPosition() + m_CurrentVelocity * Distance;

	float AngleDegrees = (rand() % 360) + 1;
	float AngleRads = AngleDegrees * M_PI / 180;

	TargetPosition.x = FuturePosition.x + (Radius * cos(AngleRads));
	TargetPosition.z = FuturePosition.z + (Radius * sin(AngleRads));
	
	Seek(TargetPosition);

	if (m_pModel->IsAtEdge())
		ReverseCurrentVelocity();
}

void Entity::LeaderFollowing()
{
	if (m_bLeaderDead)
	{
		if (m_bActive)
		{
			m_pModel->SetLeader(); // create new leader
			m_bLeaderDead = false;
		}		
	}
	
	if (m_pModel->IsLeader())
	{
		Seek(m_pModel->GetPlayerPosition());  // Leader follows player
	}

	else
	{
		if (m_pModel->IsWithinFlockingDistance(4.0f))
		{
			EvadeLeader();
		}

		else
		{
			float Distance = 75.0f;
			vec3 BackPosition = m_pModel->GetLeaderPosition() - m_CurrentLeaderVelocity * Distance; // other enemies follow the leader

			Seek(BackPosition);
		}		
	}
}

void Entity::Flocking()
{
	if (m_bLeaderDead)
	{
		if (m_bActive)
		{
			m_pModel->SetLeader(); // create new leader
			m_bLeaderDead = false;
		}
	}

	if (m_pModel->IsLeader())
	{
		Wander();
		//Seek(m_pModel->GetPlayerPosition());
	}

	else
	{
		if (m_pModel->IsWithinFlockingDistance(2.0f))
		{
			EvadeLeader();
		}
		
		else if (m_pModel->IsWithinFlockingDistance(2.5f))
		{
			m_CurrentVelocity = m_CurrentLeaderVelocity;
			//EvadeLeader();
		}

		else 
		{
			Seek(m_pModel->GetLeaderPosition());
		}
	}	
}

Model* Entity::GetModel()
{
	return m_pModel;
}

void Entity::ReverseCurrentVelocity()
{
	m_CurrentVelocity *= -1.0f;
}

vec3 Entity::GetPlayerVelocity()
{
	return m_CurrentPlayerVelocity;
}

vec3 Entity::GetCurrentVelocity()
{
	return m_CurrentVelocity;
}

void Entity::ResetCurrentVelocity()
{
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
}

EntityType Entity::GetEntityType()
{
	return m_EntityType;
}

bool Entity::IsBulletFired()
{
	return m_bBulletFired;
}

bool Entity::ToDelete()
{
	return m_pModel->ToDelete();
}

std::string Entity::GetAIName()
{
	return m_AIName;
}

vec2 Entity::GetTextPosition()
{
	return m_textPosition;
}

void Entity::SetLeaderDead()
{
	m_bLeaderDead = true;
}

void Entity::AddToScore(int _Score)
{
	m_iScore += _Score;
}

std::string Entity::GetScore()
{
	return std::to_string(m_iScore);
}

std::string Entity::GetLives()
{
	if (m_iPlayerLives < 0)
		return ("0");
	
	else
		return std::to_string(m_iPlayerLives);
}

void Entity::SetAIBehaviour(AIBehaviour _behaviour)
{
	m_Behaviour = _behaviour;
	m_AIName = _behaviour;
}

AIBehaviour Entity::GetAIBehaviour()
{
	return m_Behaviour;
}

void Entity::SetAsLeader()
{
	m_bIsLeader = true;
}

void Entity::SetAsPlayer()
{
	m_bIsPlayer = true;
}

bool Entity::IsLeader()
{
	if (m_bIsLeader)
		return true;

	else
		return false;	
}

void Entity::SetToDead()
{
	m_bIsDead = true;
}

bool Entity::IsPlayerDead()
{
	return m_bIsDead;
}

int Entity::GetPlayerLives()
{
	return m_iPlayerLives;
}

void Entity::ReducePlayerLives()
{
	m_iPlayerLives--;
}

void Entity::ResetPlayerLives()
{
	m_iPlayerLives = 3;
}

void Entity::ResetScore()
{
	m_iScore = 0;
}

bool Entity::IsActive()
{
	return m_bActive;
}

void Entity::SetActivity(bool _activity)
{
	m_bActive = _activity;

	if (!_activity)
		m_bSetBulletDirection = false;
}

void Entity::SetModelPosition(vec3 _position)
{
	m_pModel->SetPosition(_position);
}

void Entity::SetBulletFired(bool _b)
{
	m_bBulletFired = _b;
}

int Entity::GetBulletCounter()
{
	return m_iBulletCounter;
}

void Entity::IncrementBulletCounter()
{
	if (m_iBulletCounter == 9)
		m_iBulletCounter = 0;

	else
		m_iBulletCounter++;
}

void Entity::ResetVelocityForBullets()
{
	m_CurrentBulletVelocity = vec3(0.0f, 0.0f, 0.0f);
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
}

void Entity::SetMaxVelocity(float _fMaxVelocity)
{
	m_fMaxVelocity = _fMaxVelocity;
}

void Entity::SetToAlive()
{
	m_bIsDead = false;
}



