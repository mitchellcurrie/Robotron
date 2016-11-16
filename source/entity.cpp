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
//		  Juan.Rodriguez@mediadesignschool.com
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

unsigned char Entity::keyState[255];

// Static variables
//std::clock_t Entity::m_start = std::clock();
//double Entity::m_duration = 0.0f;

std::clock_t Entity::m_P1start = std::clock();
double Entity::m_P1duration = 0.0f;

std::clock_t Entity::m_P2start = std::clock();
double Entity::m_P2duration = 0.0f;

std::clock_t Entity::m_P3start = std::clock();
double Entity::m_P3duration = 0.0f;

std::clock_t Entity::m_P4start = std::clock();
double Entity::m_P4duration = 0.0f;

vec3 Entity::m_CurrentPlayerVelocity = vec3(0.0f, 0.0f, 0.0f);
vec3 Entity::m_CurrentLeaderVelocity = vec3(0.0f, 0.0f, 0.0f);
vec3 Entity::m_LastBulletVelocity = vec3(0.0f, 0.0f, -0.94167f); // -0.94 - In case the player shoots before using the shooting direction controls, shoots forward

//bool Entity::m_bBulletFired = false;
bool Entity::m_bLeaderDead = false;

//int Entity::m_iPlayerBulletCounter = 0;
int Entity::m_iEnemyBulletCounter = 0;
int Entity::m_iEnemyCounter = 0;

bool Entity::m_bP1BulletFired = false;
bool Entity::m_bP2BulletFired = false;
bool Entity::m_bP3BulletFired = false;
bool Entity::m_bP4BulletFired = false;

int Entity::m_iPlayer1BulletCounter = 0;
int Entity::m_iPlayer2BulletCounter = 0;
int Entity::m_iPlayer3BulletCounter = 0;
int Entity::m_iPlayer4BulletCounter = 0;




Entity::Entity() {
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);

	// To update:
	m_fMaxForce = 0.05f;
	m_fMaxVelocity = 0.00f;
	m_fStartingMaxVelocity = 0.00f;
	m_bSetBulletDirection = false;
	m_bIsLeader = false;
	m_bIsPlayer = false;
	m_bIsDead = false;
	m_bActive = false;
	m_bIsEnemyBullet = false;
	m_bIsSpedUp = false;
	m_bFastFire = false;
	m_bIsFleeing = false;
	/*m_iPlayerLives = 3;
	m_iScore = 0;
	m_iScoreCounter = 0;*/
	m_fFireRate = 0.12f;
	std::string m_AIName = "";
	vec2 m_textPosition = vec2(0.0f, 0.0f);
	m_PowerUp = NONPOWERUP;
	m_Behaviour = WANDER;
	m_BulletPlayer = NONPLAYERBULLET;
	m_PlayerNumber = NONPLAYER;
	srand(static_cast<unsigned int>(time(NULL)));
}

Entity::~Entity() {

}

/***********************
* Entity initialise: creates the entity based on the parameters
* Parameters: Entity Type, Model Type, number of vertices, Camera, position vector, AI Behaviour, maximum velocity float, char* filename)
* Return: void
********************/
void Entity::Initialise(EntityType _entity, ModelType _model, GLsizei _numVertices, Camera _camera, vec3 _position, AIBehaviour _behaviour, float _maxVelocity, char* _filename) {
	m_pModel = new Model;
	m_EntityType = _entity;

	if (m_bIsEnemyBullet)
		m_pModel->SetAsEnemyBullet();

	m_pModel->Initialise(_model, _numVertices, _camera, _position, IsPlayer(), IsLeader(), _filename);
	m_Behaviour = _behaviour;
	m_fMaxVelocity = _maxVelocity;
	m_fStartingMaxVelocity = _maxVelocity;

	if (m_EntityType == PLAYER) {
		if (m_PlayerNumber == P1)
			m_P1start = std::clock();

		if (m_PlayerNumber == P2)
			m_P2start = std::clock();

		if (m_PlayerNumber == P3)
			m_P3start = std::clock();

		if (m_PlayerNumber == P4)
			m_P4start = std::clock();
	}
}

/***********************
* Entity render: renders the entity by calling model render
* Parameters: none
* Return: void
********************/
void Entity::Render() {
	m_pModel->Render(m_CurrentVelocity);
}

/***********************
* Entity set positions: sets the positions of the entities by updating their velocities based on keyboard input (player and bullets) or an algorithm (AI)
* Parameters: float delta tick
* Return: void
********************/
void Entity::SetPositions(float _fDeltaTick) {

	if (m_EntityType == PLAYER) {

		if (m_PlayerNumber == P1)
			m_bP1BulletFired = false;

		else if (m_PlayerNumber == P2)
			m_bP2BulletFired = false;

		else if (m_PlayerNumber == P3)
			m_bP3BulletFired = false;

		else if (m_PlayerNumber == P4)
			m_bP4BulletFired = false;

		////////////  PLAYER Direction //////////////// Keyboard inputs ///////

		// float fSpeed = 8;
		float fSpeed = m_fMaxVelocity;

		// Forward left Movement
		if (((keyState[(unsigned char)'a'] == BUTTON_DOWN) || (keyState[(unsigned char)'A'] == BUTTON_DOWN)) &&
			((keyState[(unsigned char)'w'] == BUTTON_DOWN) || (keyState[(unsigned char)'W'] == BUTTON_DOWN))) {
			m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Forward right Movement
		else if (((keyState[(unsigned char)'d'] == BUTTON_DOWN) || (keyState[(unsigned char)'D'] == BUTTON_DOWN)) &&
				 ((keyState[(unsigned char)'w'] == BUTTON_DOWN) || (keyState[(unsigned char)'W'] == BUTTON_DOWN))) {
			m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Backward left Movement
		else if (((keyState[(unsigned char)'a'] == BUTTON_DOWN) || (keyState[(unsigned char)'A'] == BUTTON_DOWN)) &&
				 ((keyState[(unsigned char)'s'] == BUTTON_DOWN) || (keyState[(unsigned char)'S'] == BUTTON_DOWN))) {
			m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Backward right Movement
		else if (((keyState[(unsigned char)'d'] == BUTTON_DOWN) || (keyState[(unsigned char)'D'] == BUTTON_DOWN)) &&
				 ((keyState[(unsigned char)'s'] == BUTTON_DOWN) || (keyState[(unsigned char)'S'] == BUTTON_DOWN))) {
			m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Left Movement
		else if ((keyState[(unsigned char)'a'] == BUTTON_DOWN) || (keyState[(unsigned char)'A'] == BUTTON_DOWN)) {
			m_CurrentVelocity = glm::vec3(-fSpeed, 0.0f, 0.0f) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Right movement
		else if ((keyState[(unsigned char)'d'] == BUTTON_DOWN) || (keyState[(unsigned char)'D'] == BUTTON_DOWN)) {
			m_CurrentVelocity = glm::vec3(fSpeed, 0.0f, 0.0f) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Forward movement
		else if ((keyState[(unsigned char)'w'] == BUTTON_DOWN) || (keyState[(unsigned char)'W'] == BUTTON_DOWN)) {
			m_CurrentVelocity = glm::vec3(0.0f, 0.0f, -fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		// Back movement
		else if ((keyState[(unsigned char)'s'] == BUTTON_DOWN) || (keyState[(unsigned char)'S'] == BUTTON_DOWN)) {
			m_CurrentVelocity = glm::vec3(0.0f, 0.0f, fSpeed) * _fDeltaTick;
			m_pModel->SetLastPlayerVelocity(m_CurrentVelocity);
		}

		else {
			m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
		}

		////////////  BULLET Firing ///////////////////////////// Keyboard inputs ///////

		if ((keyState[(unsigned char)'j'] == BUTTON_DOWN) || (keyState[(unsigned char)'i'] == BUTTON_DOWN) ||
			(keyState[(unsigned char)'k'] == BUTTON_DOWN) || (keyState[(unsigned char)'l'] == BUTTON_DOWN) ||
			(keyState[(unsigned char)'J'] == BUTTON_DOWN) || (keyState[(unsigned char)'I'] == BUTTON_DOWN) ||
			(keyState[(unsigned char)'K'] == BUTTON_DOWN) || (keyState[(unsigned char)'L'] == BUTTON_DOWN)) {

			if (m_PlayerNumber == P1) {
				m_P1duration = (std::clock() - m_P1start) / (double)CLOCKS_PER_SEC;

				if (m_P1duration > m_fFireRate) {
					m_bP1BulletFired = true;
					m_P1start = std::clock();
				}
			}

			else if (m_PlayerNumber == P2) {
				m_P2duration = (std::clock() - m_P2start) / (double)CLOCKS_PER_SEC;

				if (m_P2duration > m_fFireRate) {
					m_bP2BulletFired = true;
					m_P2start = std::clock();
				}
			}

			else if (m_PlayerNumber == P3) {
				m_P3duration = (std::clock() - m_P3start) / (double)CLOCKS_PER_SEC;

				if (m_P3duration > m_fFireRate) {
					m_bP3BulletFired = true;
					m_P3start = std::clock();
				}
			}

			else if (m_PlayerNumber == P4) {
				m_P4duration = (std::clock() - m_P4start) / (double)CLOCKS_PER_SEC;

				if (m_P4duration > m_fFireRate) {
					m_bP4BulletFired = true;
					m_P4start = std::clock();
				}
			}
		}

		m_CurrentPlayerVelocity = m_CurrentVelocity;
	}

	////////////// Enemy movement   /////////////////////  AI algorithms   /////////////////

	else if (m_EntityType == ENEMY) {
		if (m_bIsFleeing) {
			Flee(m_pModel->GetPlayerPosition());
		}

		else {
			switch (m_Behaviour) {
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
		}

		if (m_pModel->IsLeader())
			m_CurrentLeaderVelocity = m_CurrentVelocity;
	}

	else if ((m_EntityType == BULLET) && (m_bActive)) {
		//Player Bullets
		if (!m_bIsEnemyBullet) {
			float fSpeed = 45.0f;

			// Forward left 
			if (((keyState[(unsigned char)'j'] == BUTTON_DOWN) || (keyState[(unsigned char)'J'] == BUTTON_DOWN)) &&
				((keyState[(unsigned char)'i'] == BUTTON_DOWN) || (keyState[(unsigned char)'I'] == BUTTON_DOWN))) {
				m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Forward right 
			else if (((keyState[(unsigned char)'l'] == BUTTON_DOWN) || (keyState[(unsigned char)'L'] == BUTTON_DOWN)) &&
					 ((keyState[(unsigned char)'i'] == BUTTON_DOWN) || (keyState[(unsigned char)'I'] == BUTTON_DOWN))) {
				m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Backward left 
			else if (((keyState[(unsigned char)'j'] == BUTTON_DOWN) || (keyState[(unsigned char)'J'] == BUTTON_DOWN)) &&
					 ((keyState[(unsigned char)'k'] == BUTTON_DOWN) || (keyState[(unsigned char)'K'] == BUTTON_DOWN))) {
				m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Backward right 
			else if (((keyState[(unsigned char)'l'] == BUTTON_DOWN) || (keyState[(unsigned char)'L'] == BUTTON_DOWN)) &&
					 ((keyState[(unsigned char)'k'] == BUTTON_DOWN) || (keyState[(unsigned char)'K'] == BUTTON_DOWN))) {
				m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Left 
			else if ((keyState[(unsigned char)'j'] == BUTTON_DOWN) || (keyState[(unsigned char)'J'] == BUTTON_DOWN)) {
				m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, 0.0f) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Right 
			else if ((keyState[(unsigned char)'l'] == BUTTON_DOWN) || (keyState[(unsigned char)'L'] == BUTTON_DOWN)) {
				m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, 0.0f) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Forward 
			else if ((keyState[(unsigned char)'i'] == BUTTON_DOWN) || (keyState[(unsigned char)'I'] == BUTTON_DOWN)) {
				m_CurrentBulletVelocity = glm::vec3(0.0f, 0.0f, -fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Back 
			else if ((keyState[(unsigned char)'k'] == BUTTON_DOWN) || (keyState[(unsigned char)'K'] == BUTTON_DOWN)) {
				m_CurrentBulletVelocity = glm::vec3(0.0f, 0.0f, fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			else {
				m_CurrentBulletVelocity = glm::vec3(0.0f, 0.0f, -fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}
		}
		//Enemy Bullets
		else {
			float fSpeed = 9.0f;

			int iRandom = rand() % 8;

			// Forward left 
			if (iRandom == 0) {
				m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Forward right 
			else if (iRandom == 1) {
				m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, -fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Backward left 
			else if (iRandom == 2) {
				m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Backward right 
			else if (iRandom == 3) {
				m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Left 
			else if (iRandom == 4) {
				m_CurrentBulletVelocity = glm::vec3(-fSpeed, 0.0f, 0.0f) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Right 
			else if (iRandom == 5) {
				m_CurrentBulletVelocity = glm::vec3(fSpeed, 0.0f, 0.0f) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Forward 
			else if (iRandom == 6) {
				m_CurrentBulletVelocity = glm::vec3(0.0f, 0.0f, -fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}

			// Back 
			else if (iRandom == 7) {
				m_CurrentBulletVelocity = glm::vec3(0.0f, 0.0f, fSpeed) * _fDeltaTick;
				m_LastBulletVelocity = m_CurrentBulletVelocity;
			}
		}

		if (!m_bSetBulletDirection) {
			// m_CurrentVelocity = m_pModel->GetLastPlayerVelocity() * 7.0f;
			m_CurrentVelocity = m_LastBulletVelocity;
			m_bSetBulletDirection = true;
		}
	}

	else if ((m_EntityType == POWERUP) && (m_bActive)) {
		Wander();
	}
}

bool Entity::IsPlayer() {
	if (m_EntityType == PLAYER)
		return true;

	else
		return false;
}

void Entity::KeyDown(unsigned char key, int x, int y) {
	keyState[key] = BUTTON_DOWN;
}

void Entity::KeyUp(unsigned char key, int x, int y) {
	keyState[key] = BUTTON_UP;
}

/***********************
* Entity limit force: limits the force applied to the object for the purposes of calculating its new velocity
* Parameters: steering force vector
* Return: vec3 vector that's limited
********************/
vec3 Entity::LimitForce(vec3 _steering) {
	vec3 Steering = _steering;

	if (_steering.x > m_fMaxForce)
		Steering.x = m_fMaxForce;

	if (_steering.y > m_fMaxForce)
		Steering.y = m_fMaxForce;

	if (_steering.z > m_fMaxForce)
		Steering.z = m_fMaxForce;

	return Steering;
}

/***********************
* Entity limit velocity: limits the velocity applied to the object for the purposes of calculating its new velocity
* Parameters: velocity vector
* Return: vec3 vector that's limited
********************/
vec3 Entity::LimitVelocity(vec3 _velocity) {
	vec3 Velocity = _velocity;

	if (_velocity.x > m_fMaxVelocity)
		Velocity.x = m_fMaxVelocity;

	if (_velocity.y > m_fMaxVelocity)
		Velocity.y = m_fMaxVelocity;

	if (_velocity.z > m_fMaxVelocity)
		Velocity.z = m_fMaxVelocity;

	return Velocity;
}

/***********************
* Entity seek: AI algorithm where the objects move towards another objects current position
* Parameters: vec3 position - of what is being seeked
* Return: void
********************/
void Entity::Seek(vec3 _position) {
	vec3 DesiredVelocity = _position - m_pModel->GetPosition();
	DesiredVelocity = glm::normalize(DesiredVelocity);

	DesiredVelocity *= m_fMaxVelocity;

	vec3 SteeringForce = DesiredVelocity - m_CurrentVelocity;
	SteeringForce = LimitForce(SteeringForce);

	m_CurrentVelocity += SteeringForce;
	m_CurrentVelocity = LimitVelocity(m_CurrentVelocity);
}

/***********************
* Entity flee: AI algorithm where the objects move away from another objects current position
* Parameters: vec3 position - of what is being fleed
* Return: void
********************/
void Entity::Flee(vec3 _position) {
	vec3 DesiredVelocity = m_pModel->GetPosition() - _position;
	DesiredVelocity = glm::normalize(DesiredVelocity);

	DesiredVelocity *= m_fMaxVelocity;

	vec3 SteeringForce = DesiredVelocity - m_CurrentVelocity;
	SteeringForce = LimitForce(SteeringForce);

	m_CurrentVelocity += SteeringForce;
	m_CurrentVelocity = LimitVelocity(m_CurrentVelocity);
}

/***********************
* Entity pursue: AI algorithm where the objects move towards another objects future position
* Parameters: vec3 position - of what is being pursued
* Return: void
********************/
void Entity::Pursue() {
	vec3 Distance = m_pModel->GetPlayerPosition() - m_pModel->GetPosition();

	//float T = Distance.length() / (m_fMaxVelocity + GetPlayerVelocity().length());
	float T = Distance.length() / m_fMaxVelocity;
	//float T = length(Distance) / m_fMaxVelocity;
	// float T = 50.0f;

	vec3 FuturePosition = m_pModel->GetPlayerPosition() + (m_CurrentPlayerVelocity * T);

	if (m_pModel->IsWithinPlayerRange(3.0f)) // To stop the enemy moving away from the player as the player gets close
		Seek(m_pModel->GetPlayerPosition());

	else
		Seek(FuturePosition);

}

/***********************
* Entity evade: AI algorithm where the objects move away from another objects future position
* Parameters: vec3 position - of what is being evaded
* Return: void
********************/
void Entity::Evade() {
	vec3 Distance = m_pModel->GetPlayerPosition() - m_pModel->GetPosition();

	float T = Distance.length() / m_fMaxVelocity;
	//float T = 50.0f;

	vec3 FuturePosition = m_pModel->GetPlayerPosition() + m_CurrentPlayerVelocity * T;

	if (m_pModel->IsWithinPlayerRange(6.0f)) // To stop the enemy moving towards the player as the player gets close - as they are moving in the opposite direction
		Flee(m_pModel->GetPlayerPosition());

	else
		Flee(FuturePosition);

}

/***********************
* Entity evade: AI algorithm evade, similar to above instead this is for enemies evading the leader enemy - for when the leader is passing through other enemies
* Parameters: none
* Return: void
********************/
void Entity::EvadeLeader() {
	vec3 Distance = m_pModel->GetLeaderPosition() - m_pModel->GetPosition();

	float T = Distance.length() / m_fMaxVelocity;
	//float T = 50.0f;

	vec3 FuturePosition = m_pModel->GetLeaderPosition() + m_CurrentLeaderVelocity * T;

	Flee(FuturePosition);
}

/***********************
* Entity wander: AI algorithm where objects appear to move randomly around the map in a way that doesn't appear unusual
* Parameters: none
* Return: void
********************/
void Entity::Wander() {
	float Distance = 1500.0f;
	float Radius = 7.0f;

	if (m_EntityType == POWERUP) // For power up wandering
	{
		float Distance = 3000.0f;
		float Radius = 0.05f;
	}

	vec3 TargetPosition = vec3(0.0f, 0.0f, 0.0f);

	vec3 FuturePosition = m_pModel->GetPosition() + m_CurrentVelocity * Distance;

	int AngleDegrees = (rand() % 360) + 1;
	float AngleRads = static_cast<float>(AngleDegrees) * static_cast<float>(M_PI) / 180.0f;

	TargetPosition.x = FuturePosition.x + (Radius * cos(AngleRads));
	TargetPosition.z = FuturePosition.z + (Radius * sin(AngleRads));

	Seek(TargetPosition);

	if (m_pModel->IsAtEdge())
		ReverseCurrentVelocity();
}

/***********************
* Entity leader following: AI algorithm where the enemies seek the enemy leader's position, after subtracting a distance, and the enemy leader seeks the player
* Parameters: none
* Return: void
********************/
void Entity::LeaderFollowing() {
	// If the leader is dead
	if (m_bLeaderDead) {
		if (m_bActive) {
			m_pModel->SetLeader(true); // create new leader
			m_bLeaderDead = false;
		}
	}

	if (m_pModel->IsLeader()) {
		Seek(m_pModel->GetPlayerPosition());  // Leader seeks player
	}

	else {
		if (m_pModel->IsWithinFlockingDistance(4.0f)) {
			EvadeLeader();  // If enemies are too close to the leader, evade to let them through
		}

		else {
			float Distance = 75.0f;
			vec3 BackPosition = m_pModel->GetLeaderPosition() - m_CurrentLeaderVelocity * Distance; // other enemies follow the leader

			Seek(BackPosition);
		}
	}
}

/***********************
* Entity flocking: AI algorithm where the objects form a group and wander the map together
* Parameters: none
* Return: void
********************/
void Entity::Flocking() {
	if (m_bLeaderDead) {
		if (m_bActive) {
			m_pModel->SetLeader(true); // create new leader
			m_bLeaderDead = false;
		}
	}

	if (m_pModel->IsLeader()) {
		Wander();
		//Seek(m_pModel->GetPlayerPosition());
	}

	else {
		if (m_pModel->IsWithinFlockingDistance(2.0f)) {
			EvadeLeader(); // If too close, evade
		}

		else if (m_pModel->IsWithinFlockingDistance(2.5f)) {
			m_CurrentVelocity = m_CurrentLeaderVelocity;
			//EvadeLeader();
		}

		else {
			Seek(m_pModel->GetLeaderPosition());  // If too far away, seek
		}
	}
}

Model* Entity::GetModel() {
	return m_pModel;
}

void Entity::ReverseCurrentVelocity() {
	m_CurrentVelocity *= -1.0f;
}

vec3 Entity::GetPlayerVelocity() {
	return m_CurrentPlayerVelocity;
}

vec3 Entity::GetCurrentVelocity() {
	return m_CurrentVelocity;
}

void Entity::ResetCurrentVelocity() {
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
}

EntityType Entity::GetEntityType() {
	return m_EntityType;
}

//bool Entity::IsBulletFired()
//{
//	return m_bBulletFired;
//}

bool Entity::IsP1BulletFired() {
	return m_bP1BulletFired;
}

bool Entity::IsP2BulletFired() {
	return m_bP2BulletFired;
}

bool Entity::IsP3BulletFired() {
	return m_bP3BulletFired;
}

bool Entity::IsP4BulletFired() {
	return m_bP4BulletFired;
}

bool Entity::ToDelete() {
	return m_pModel->ToDelete();
}

std::string Entity::GetAIName() {
	return m_AIName;
}

vec2 Entity::GetTextPosition() {
	return m_textPosition;
}

void Entity::SetLeaderDead() {
	m_bLeaderDead = true;
}

//void Entity::AddToScore(int _Score)
//{
//	m_iScore += _Score;
//	m_iScoreCounter += _Score;
//}
//
//std::string Entity::GetScore()
//{
//	return std::to_string(m_iScore);
//}
//
//std::string Entity::GetLives()
//{
//	if (m_iPlayerLives < 0)
//		return ("0");
//
//	else
//		return std::to_string(m_iPlayerLives);
//}

void Entity::SetAIBehaviour(AIBehaviour _behaviour) {
	m_Behaviour = _behaviour;
	m_AIName = _behaviour;
}

AIBehaviour Entity::GetAIBehaviour() {
	return m_Behaviour;
}

void Entity::SetLeader(bool _b) {
	m_bIsLeader = _b;
	//	m_pModel->SetLeader(_b);
}

void Entity::SetAsPlayer() {
	m_bIsPlayer = true;
}

bool Entity::IsLeader() {
	if (m_bIsLeader)
		return true;

	else
		return false;
}

void Entity::SetToDead() {
	m_bIsDead = true;
}

bool Entity::IsPlayerDead() {
	return m_bIsDead;
}

//int Entity::GetPlayerLives()
//{
//	return m_iPlayerLives;
//}
//
//void Entity::ReducePlayerLives()
//{
//	m_iPlayerLives--;
//}
//
//void Entity::ResetPlayerLives()
//{
//	m_iPlayerLives = 3;
//}

//void Entity::ResetScore()
//{
//	m_iScore = 0;
//	m_iScoreCounter = 0;
//}
//
//void Entity::ResetScoreCounter()
//{
//	m_iScoreCounter = 0;
//}
//
//int Entity::GetScoreCounter()
//{
//	return m_iScoreCounter;
//}

bool Entity::IsActive() {
	return m_bActive;
}

void Entity::SetActive(bool _activity) {
	m_bActive = _activity;

	if (!_activity)
		m_bSetBulletDirection = false;
}

void Entity::SetModelPosition(vec3 _position) {
	m_pModel->SetPosition(_position);
}

void Entity::SetP1BulletFired(bool _b) {
	m_bP1BulletFired = _b;
}

void Entity::SetP2BulletFired(bool _b) {
	m_bP2BulletFired = _b;
}

void Entity::SetP3BulletFired(bool _b) {
	m_bP3BulletFired = _b;
}

void Entity::SetP4BulletFired(bool _b) {
	m_bP4BulletFired = _b;
}


int Entity::GetPlayer1BulletCounter() {
	return m_iPlayer1BulletCounter;
}

int Entity::GetPlayer2BulletCounter() {
	return m_iPlayer2BulletCounter;
}

int Entity::GetPlayer3BulletCounter() {
	return m_iPlayer3BulletCounter;
}

int Entity::GetPlayer4BulletCounter() {
	return m_iPlayer4BulletCounter;
}

//void Entity::SetBulletFired(bool _b)
//{
//	m_bBulletFired = _b;
//}
//
//int Entity::GetPlayerBulletCounter()
//{
//	return m_iPlayerBulletCounter;
//}

int Entity::GetEnemyBulletCounter() {
	return m_iEnemyBulletCounter;
}

int Entity::GetEnemyCounter() {
	return m_iEnemyCounter;
}

//void Entity::IncrementPlayerBulletCounter()
//{
//	if (m_iPlayerBulletCounter == 9)
//		m_iPlayerBulletCounter = 0;
//
//	else
//		m_iPlayerBulletCounter++;
//}

void Entity::IncrementPlayer1BulletCounter() {
	if (m_iPlayer1BulletCounter == 9)
		m_iPlayer1BulletCounter = 0;

	else
		m_iPlayer1BulletCounter++;
}

void Entity::IncrementPlayer2BulletCounter() {
	if (m_iPlayer2BulletCounter == 9)
		m_iPlayer2BulletCounter = 0;

	else
		m_iPlayer2BulletCounter++;
}

void Entity::IncrementPlayer3BulletCounter() {
	if (m_iPlayer3BulletCounter == 9)
		m_iPlayer3BulletCounter = 0;

	else
		m_iPlayer3BulletCounter++;
}

void Entity::IncrementPlayer4BulletCounter() {
	if (m_iPlayer4BulletCounter == 9)
		m_iPlayer4BulletCounter = 0;

	else
		m_iPlayer4BulletCounter++;
}

void Entity::IncrementEnemyBulletCounter() {
	if (m_iEnemyBulletCounter == 39)
		m_iEnemyBulletCounter = 0;

	else
		m_iEnemyBulletCounter++;
}

void Entity::IncrementEnemyCounter() {
	if (m_iEnemyCounter == 34)
		m_iEnemyCounter = 0;

	else
		m_iEnemyCounter++;
}

void Entity::ResetVelocityForPlayerBullets() {
	m_CurrentBulletVelocity = vec3(0.0f, 0.0f, 0.0f);
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
}

void Entity::VelocityToZero() {
	m_CurrentVelocity = vec3(0.0f, 0.0f, 0.0f);
}

void Entity::SetMaxVelocity(float _fMaxVelocity) {
	m_fMaxVelocity = _fMaxVelocity;
}

void Entity::SetToAlive() {
	m_bIsDead = false;
}

void Entity::SetAsEnemyBullet() {
	m_bIsEnemyBullet = true;
}

void Entity::ResetEnemyCounter() {
	m_iEnemyCounter = 0;
}

void Entity::SetModelOutsideMap(bool _b) {
	m_pModel->SetOutsideMap(_b);
}

void Entity::SetPowerUpType(PowerUpType _type) {
	m_PowerUp = _type;
}

void Entity::ResetMaxVelocity() {
	m_fMaxVelocity = m_fStartingMaxVelocity;
}

void Entity::SpeedUp() {
	m_fMaxVelocity = 12.0f;
}

PowerUpType Entity::GetPowerUpType() {
	return m_PowerUp;
}

void Entity::SetSpedUp(bool _b) {
	m_bIsSpedUp = _b;
}

bool Entity::IsSpedUp() {
	return m_bIsSpedUp;
}

void Entity::FastFire() {
	m_fFireRate = 0.05f;
}
void Entity::SetFastFire(bool _b) {
	m_bFastFire = _b;
}

bool Entity::IsFiringFast() {
	return m_bFastFire;
}

void Entity::ResetFireRate() {
	m_fFireRate = 0.12f;
}

//void Entity::AddExtraLife()
//{
//	m_iPlayerLives++;
//}

bool Entity::IsFleeing() {
	return m_bIsFleeing;
}

void Entity::SetEnemyToFlee(bool _b) {
	m_bIsFleeing = _b;
}

void Entity::SetBulletPlayer(BulletPlayer _player) {
	m_BulletPlayer = _player;
}

BulletPlayer Entity::GetBulletPlayer() {
	return m_BulletPlayer;
}

void Entity::SetPlayerNumber(PlayerNumber _number) {
	m_PlayerNumber = _number;
}

PlayerNumber Entity::GetPlayerNumber() {
	return m_PlayerNumber;
}

//bool Entity::IsEnemyFiring()
//{
//	return m_bEnemyToFire;
//}



