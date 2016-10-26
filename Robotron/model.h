//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : model.h
// Description : Model header file - sets indices and vertices, VBO, VAO, EBO, performs rotations and updates model position
// Mail : Mitchell.Currie@mediadesignschool.com
//

#pragma once

#include "utils.h"
#include "camera.h"

#define BUTTON_UP   0
#define BUTTON_DOWN 1

class Model
{
public:

	Model();
	~Model();
	void Initialise(ModelType _model, GLsizei _numVertices, Camera _camera, vec3 _position, bool _IsPlayer);
	void Render(vec3 _CurrentVelocity);
	void Rotate();
	ModelType GetModelType();
	vec3 GetPosition();
	vec3 GetPlayerPosition();

private:

	GLuint program;
	GLuint vao, vbo, ebo;
	GLuint texture, texture2;
	vec3 m_position;
	GLsizei m_numVertices;
	Camera m_Camera;
	ModelType m_ModelType;

	static vec3 m_PlayerPosition;

	bool m_IsPlayer;
};
