//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : utils.h
// Description : Utils header file - creates model type enum, sets and gets vertices and indices
// Mail : Mitchell.Currie@mediadesignschool.com
//

#pragma once

#define MAP_SIZE 17.0f

#define VALIDATE(a) if (!a) return (false)

#include <strstream>
#include "glew.h"
#include "freeglut.h"

#include <ft2build.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

struct Player
{
	// Variables
	glm::vec3 m_Position;
	bool m_bIsActive;
	char* m_name;

	// Functions
	void SetPosition(glm::vec3 _position){
		m_Position = _position;
	}

	glm::vec3 GetPosition() {
		return m_Position;
	}

	void SetActivity(bool _b) {
		m_bIsActive = _b;
	}

	bool IsActive() {
		return m_bIsActive;
	}

	void SetName(char* _name) {
	//	strcpy_s(m_name, strlen(_name) + 1, _name);
		m_name = _name;
	}

	char* GetName() {
		return m_name;
	}
};

struct Enemy {
	// Variables
	glm::vec3 m_Position;
	bool m_bIsActive;

	// Functions
	void SetPosition(glm::vec3 _position) {
		m_Position = _position;
	}

	glm::vec3 GetPosition() {
		return m_Position;
	}

	void SetActivity(bool _b) {
		m_bIsActive = _b;
	}

	bool IsActive() {
		return m_bIsActive;
	}
};

struct Position
{
	// Not required
};

struct TexCoord
{
	// Not required
};

struct VertexFormat
{
	// Not required
};

struct IndexFormat
{
	// Not required 
};

enum GameState 
{
	MENU,
	NETWORK_SELECT,
	PLAY,
	CREDITS,
	GAME_OVER,
	GAME_QUIT
};

enum ModelType
{
	TRIANGLE,
	QUAD,
	CUBE,
	PYRAMID,
	OCTAGON,
	DOT
};

enum EntityType
{
	PLAYER,
	ENEMY,
	MAP,
	BULLET,
	POWERUP
};

enum TextType
{
	M_LABEL,
	M_CURSOR,
	NS_LABEL,
	NS_CURSOR,
	SERVER_LABEL,
	CLIENT_LABEL,
	CLIENT_CURSOR,
	P_LABEL,
	P_AIDESCRIPTION,
	P_SCORE,
	P_LEVEL,
	P_LIVES,
	C_LABEL,
	G_LABEL,
	G_SCORE,
	G_LEVEL,
	NONE
};

enum AIBehaviour
{
	NONENEMY,
	SEEK,
	FLEE,
	PURSUE,
	EVADE,
	WANDER,
	LEADERFOLLOW,
	FLOCK
};

enum PowerUpType
{
	NONPOWERUP,
	SPEEDBOOST,
	FASTFIRING,
	ENEMYFLEE
};

enum BulletPlayer {
	NONPLAYERBULLET,
	BP1,
	BP2,
	BP3,
	BP4
};

enum PlayerNumber {
	NONPLAYER,
	P1,
	P2,
	P3,
	P4
};

enum Color {
	BLACK = 0,
	DBLUE,
	DGREEN,
	DTEAL,
	DRED,
	DPINK,
	BROWN,
	GRAY,
	DGRAY,
	BLUE,
	GREEN,
	TEAL,
	RED,
	PINK,
	YELLOW,
	WHITE
};

template<typename T>
std::string ToString(const T& _value) {
	std::strstream theStream;
	theStream << _value << std::ends;
	return (theStream.str());
}

void SetColor(Color _color);

class Utils
{
public:

	Utils();
	~Utils();
//	static void SetVerticesAndIndices(GLfloat* _vertices, GLuint* _indices, ModelType _model);
	static void SetVerticesAndIndices(ModelType _model);
	static GLfloat* GetVertices();
	static GLuint* GetIndices();

	static GLfloat* m_vertices;
	static GLuint* m_indices;
	static int m_sizeVertices;
	static int m_sizeIndices;
};


