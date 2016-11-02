//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : utils.cpp
// Description : Utils implementation file - creates model type enum, sets and gets vertices and indices
// Mail : Mitchell.Currie@mediadesignschool.com
//

#include <iostream>
#include "glew.h"
#include "freeglut.h"

#include "utils.h"

GLfloat* Utils::m_vertices{ 0 };
GLuint* Utils::m_indices{ 0 };
int Utils::m_sizeVertices{ 0 };
int Utils::m_sizeIndices{ 0 };

Utils::Utils()
{

}
Utils::~Utils()
{

}

/***********************
* Utils Set vertices and indices: sets the vertices and indices of the model based on the model type
* Parameters: ModelType enum
* Return: void
********************/
void Utils::SetVerticesAndIndices (ModelType _model)
{
	m_sizeVertices = 0;
	m_sizeIndices = 0;

	if (_model == TRIANGLE)
	{
		//Triangle	
		GLfloat vertices[]{

			0.0f,  0.5774f, 0.0f, 1.0f, 0.0f, 0.0f,  // Top  
	  	   -0.5f, -0.2887f, 0.0f, 0.0f, 1.0f, 0.0f,  // Bottom left 
			0.5f, -0.2887f, 0.0f, 0.0f, 0.0f, 1.0f,  // Bottom right

		   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};

		GLuint indices[]{
			0, 1, 2  // Triangle
		};


		m_vertices = vertices;
		m_indices = indices;

		m_sizeVertices = sizeof(vertices) / 4;
		m_sizeIndices = sizeof(indices) / 4;
	}

	else if (_model == QUAD)
	{
		float fMapSize = MAP_SIZE;
		// Quad
		GLfloat vertices[]{
			-fMapSize, -0.5f,  fMapSize, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,		// Top right  
			-fMapSize, -0.5f, -fMapSize, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,		// Bottom right 
			 fMapSize, -0.5f, -fMapSize, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,		// Bottom left
			 fMapSize, -0.5f,  fMapSize, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,		// Top left

			 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};

		GLuint indices[]{
			2, 1, 0, // First Triangle
			2, 3, 0  // Second Triangle

		};

		m_vertices = vertices;
		m_indices = indices;

		m_sizeVertices = sizeof(vertices) / 4;
		m_sizeIndices = sizeof(indices) / 4;
	}


	else if (_model == CUBE)
	{
		// Cube

			GLfloat vertices[]{

			//position		//color					//texture coord
			-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -0.0f, 0.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, -0.0f, 1.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,

			// Fill in the back face vertex data.
			-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

			// Fill in the top face vertex data.
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

			// Fill in the bottom face vertex data.
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

			// Fill in the left face vertex data.
			-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

			// Fill in the right face vertex data.
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
		};

			GLuint indices[]{
			// front
			0, 1, 2,
			0, 2, 3,
			// top
			4, 5, 6,
			4, 6, 7,
			//0, 1, 8,
			//1, 8,9,
			// back
			8, 9, 10,
			8, 10, 11,
			// bottom
			12, 13, 14,
			12, 14, 15,
			// left
			16, 17, 18,
			16, 18, 19,
			// right
			20, 21, 22,
			20, 22, 23
		};

		m_vertices = vertices;
		m_indices = indices;	

		m_sizeVertices = sizeof(vertices) / 4;
		m_sizeIndices = sizeof(indices) / 4;
	}		

	else if (_model == PYRAMID)
	{
		// Pyramid	

		float fSize = 0.4f;

		GLfloat vertices[]{
		   -fSize, -fSize, -fSize, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // left back
			fSize, -fSize, -fSize, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // right back
			fSize, -fSize,  fSize, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // right front
		   -fSize, -fSize,  fSize, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // left front
		     0.0f,  fSize,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top

			 0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f  // bottom - used if we add another pyramid upside down	
		};

		GLuint indices[]{
			2, 4, 3, // front
			4, 2, 1, // right
			0, 4, 1, // back
			4, 0, 3, // left
			0, 1, 2, // bottom
			0, 2, 3 // bottom
		};

		m_vertices = vertices;
		m_indices = indices;

		m_sizeVertices = sizeof(vertices) / 4;
		m_sizeIndices = sizeof(indices) / 4;
	}

	else if (_model == OCTAGON)
	{
		//Octagon	
		GLfloat vertices[]{

			    0.0f,    0.5f,   0.0f, 1.0f, 1.0f, 1.0f,  // Top  
			   -0.5f,   -0.0f,   0.0f, 0.0f, 0.0f, 0.0f,  // Middle left 
				0.5f,   -0.0f,   0.0f, 0.0f, 0.0f, 1.0f,  // Middle right
				0.0f,   -0.5f,   0.0f, 1.0f, 1.0f, 1.0f,  // Bottom
				0.375f,  0.375f, 0.0f, 1.0f, 0.0f, 0.0f,  // top right
			   -0.375f,  0.375f, 0.0f, 1.0f, 0.0f, 1.0f,  // top left
			    0.375f, -0.375f, 0.0f, 1.0f, 0.0f, 1.0f,  // bottom right
			   -0.375f, -0.375f, 0.0f, 1.0f, 0.0f, 1.0f,  // bottom left
		};

		GLuint indices[]{
			1, 2, 0,  
			1, 2, 3,
			0, 4, 2,
			0, 5, 1,
			2, 6, 3,
			1, 7, 3
		};


		m_vertices = vertices;
		m_indices = indices;

		m_sizeVertices = sizeof(vertices) / 4;
		m_sizeIndices = sizeof(indices) / 4;
	}

	else if (_model == DOT)
	{
		float fBulletSize = 0.1f;
		// Quad
		GLfloat vertices[]{
				fBulletSize, 0.0f,  fBulletSize, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,		// Top right  
				fBulletSize, 0.0f, -fBulletSize, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,		// Bottom right 
			   -fBulletSize, 0.0f, -fBulletSize, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,		// Bottom left
			   -fBulletSize, 0.0f,  fBulletSize, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,		// Top left

				0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};

		GLuint indices[]{
			2, 1, 0, // First Triangle
			2, 3, 0  // Second Triangle

		};

		m_vertices = vertices;
		m_indices = indices;

		m_sizeVertices = sizeof(vertices) / 4;
		m_sizeIndices = sizeof(indices) / 4;
	}

}


GLfloat* Utils::GetVertices()
{
	return m_vertices;
}

GLuint* Utils::GetIndices()
{
	return m_indices;
}

