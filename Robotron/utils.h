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

template<typename T>
std::string ToString(const T& _value)
{
	std::strstream theStream;
	theStream << _value << std::ends;
	return (theStream.str());
}

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
	P_LABEL,
	P_AIDESCRIPTION,
	P_SCORE,
	P_LEVEL,
	P_LIVES,
	M_LABEL,
	M_CURSOR,
	G_LABEL,
	G_SCORE,
	G_LEVEL,
	C_LABEL

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


