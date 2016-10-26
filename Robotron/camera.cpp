//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : camera.cpp
// Description : Camera implementation file - creates perspective camera
// Mail : Mitchell.Currie@mediadesignschool.com
//


#include <iostream>
#include "glew.h"
#include "freeglut.h"

#include "camera.h"

Camera::Camera()
{

}


Camera::Camera(vec3 _cameraPos, vec3 _cameraFront, vec3 _cameraUp, GLfloat _width, GLfloat _height)
{
	m_cameraPos = _cameraPos;
	m_cameraFront = _cameraFront;
	m_cameraUp = _cameraUp;
	m_width = _width;
	m_height = _height;
}

Camera::~Camera()
{

}

mat4 Camera::GetProjection()
{
	// Creates perspective camera
	mat4 projection = perspective(45.0f, (GLfloat)m_width / (GLfloat)m_height, 0.1f, 100.0f);

	return projection;
}

mat4 Camera::GetView()
{
	mat4 view = lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

	return view;
}

