//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : camera.h
// Description : Camera header file - creates perspective camera
// Mail : Mitchell.Currie@mediadesignschool.com
//

#pragma once

#include "glew.h"
#include "freeglut.h"
#include "SOIL.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

class Camera
{
public:

	Camera();
	Camera(vec3 _cameraPos, vec3 _cameraFront, vec3 _cameraUp, GLfloat _width, GLfloat _height);
	~Camera();
	mat4 GetProjection();
	mat4 GetView();

private:

	vec3 m_cameraPos;
	vec3 m_cameraFront;
	vec3 m_cameraUp;
	GLfloat m_width;
	GLfloat m_height;
};
