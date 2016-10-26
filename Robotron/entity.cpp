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

#include <iostream>
#include "glew.h"
#include "freeglut.h"
#include "entity.h"
#include "ShaderLoader.h"
#include "utils.h"

unsigned char keyState[255];

vec3 Entity::m_CurrentPlayerVelocity = vec3(0.0f, 0.0f, 0.0f);


Entity::Entity()
{
	m_IsPlayer = false;
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);

	// To update:
	m_fMaxForce = 0.05f;
	m_fMaxVelocity = 0.05f;
}

Entity::~Entity()
{

}

void Entity::Initialise(ModelType _model, GLsizei _numVertices, Camera _camera, vec3 _position, bool _IsPlayer)
{
	m_pModel = new Model;
	m_pModel->Initialise(_model, _numVertices, _camera, _position, _IsPlayer);
	m_IsPlayer = _IsPlayer;
}


void Entity::Render()
{
	m_pModel->Render(m_CurrentVelocity);
}

void Entity::SetPositions(float _fDeltaTick)
{
	if (m_pModel->GetModelType() == CUBE)
	{
		if (m_IsPlayer)
		{
			float fSpeed = 8;

			// Forward left Movement
			if ((keyState[(unsigned char)'a'] == BUTTON_DOWN) && (keyState[(unsigned char)'w'] == BUTTON_DOWN))
			{
				printf("Move cube forward left \n");
				m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
			}

			// Forward right Movement
			else if ((keyState[(unsigned char)'d'] == BUTTON_DOWN) && (keyState[(unsigned char)'w'] == BUTTON_DOWN))
			{
				printf("Move cube forward right \n");
				m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
			}

			// Backward left Movement
			else if ((keyState[(unsigned char)'a'] == BUTTON_DOWN) && (keyState[(unsigned char)'s'] == BUTTON_DOWN))
			{
				printf("Move cube backward left \n");
				m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, fSpeed) * _fDeltaTick;
			}

			// Backward right Movement
			else if ((keyState[(unsigned char)'d'] == BUTTON_DOWN) && (keyState[(unsigned char)'s'] == BUTTON_DOWN))
			{
				printf("Move cube backward right \n");
				m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, fSpeed) * _fDeltaTick;
			}

			// Left Movement
			else if (keyState[(unsigned char)'a'] == BUTTON_DOWN)
			{
				printf("Move cube left \n");
				m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, 0.0f) * _fDeltaTick;
			}

			// Right movement
			else if (keyState[(unsigned char)'d'] == BUTTON_DOWN)
			{
				printf("Move cube right \n");
				m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, 0.0f) * _fDeltaTick;
			}

			// Forward movement
			else if (keyState[(unsigned char)'w'] == BUTTON_DOWN)
			{
				printf("Move cube forward \n");
				m_CurrentVelocity = glm::vec3(0.0f, 0.0f, -fSpeed) * _fDeltaTick;
			}

			// Back movement
			else if (keyState[(unsigned char)'s'] == BUTTON_DOWN)
			{
				printf("Move cube back \n");
				m_CurrentVelocity = glm::vec3(0.0f, 0.0f, fSpeed) * _fDeltaTick;
			}

			else
			{
				m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
				// Stop moving - if another window pops up etc 
			}

			m_CurrentPlayerVelocity = m_CurrentVelocity;
		}

		else // Enemy movement
		{
			
		//	Seek(m_pModel->GetPlayerPosition());
		//	Flee(m_pModel->GetPlayerPosition());
			Pursue();
		//	Evade();
		}

	} // end if model type cube
}

bool Entity::IsPlayer()
{
	if (m_IsPlayer)
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

	float T = Distance.length() / m_fMaxVelocity;
	//float T = 50.0f;

	vec3 FuturePosition = m_pModel->GetPlayerPosition() + GetPlayerVelocity() * T;

	Seek(FuturePosition);
}

void Entity::Evade()
{
	vec3 Distance = m_pModel->GetPlayerPosition() - m_pModel->GetPosition();

	float T = Distance.length() / m_fMaxVelocity;
	//float T = 50.0f;

	vec3 FuturePosition = m_pModel->GetPlayerPosition() + GetPlayerVelocity() * T;

	Flee(FuturePosition);
}

Model* Entity::GetModel()
{
	return m_pModel;
}

void Entity::ResetCurrentVelocity()
{
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
}

vec3 Entity::GetPlayerVelocity()
{
	return m_CurrentPlayerVelocity;
}
