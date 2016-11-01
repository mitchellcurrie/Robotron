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
	STATIC,
	AIDESCRIPTION,
	SCORE,
	LEVEL,
	LIVES
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
