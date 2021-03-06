//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : model.cpp
// Description : Model implementation file
// Mail : Mitchell.Currie@mediadesignschool.com
//		  Juan.Rodriguez@mediadesignschool.com
//

#pragma once

#include <iostream>
#include "glew.h"
#include "freeglut.h"
#include "model.h"
#include "ShaderLoader.h"
#include "utils.h"

// Static variables
vec3 Model::m_PlayerPosition = vec3(0.0f, 0.0f, 0.0f);
vec3 Model::m_LeaderPosition = vec3(0.0f, 0.0f, 0.0f);
vec3 Model::m_LastPlayerVelocity = vec3(0.0f, 0.0f, 0.0f);
// unsigned char keyState[255];

Model::Model()
{
	m_IsPlayer = false;
	m_IsLeader = false;
	m_bToBeDeleted = false;
	m_bOutsideMap = false;
	m_bIsEnemyBullet = false;
	m_pcFileName = "";
}

Model::~Model()
{

}

/***********************
* Model initialise: creates the model based on the parameters
* Parameters: ModelType enum, number of vertices, camera, position vector, bool is player, bool is leader, char* filename 
* Return: void
********************/
void Model::Initialise(ModelType _model, GLsizei _numVertices, Camera _camera, vec3 _position, bool _IsPlayer, bool _IsLeader, char* _filename)
{
	m_IsPlayer = _IsPlayer;
	m_IsLeader = _IsLeader;

	m_pcFileName = _filename;

	Utils::SetVerticesAndIndices(_model);

	GLfloat* verticesAddress = Utils::GetVertices();
	GLuint* indicesAddress = Utils::GetIndices();
	
	// Cube maximums - covers other shapes
	GLfloat Vertices[192] { 0 };
	GLuint Indices[36]  { 0 };


	int iSize{ 0 };   // for vertex and colour attributes

	for (int x{ 0 }; x < Utils::m_sizeVertices; x++)
	{
		Vertices[x] = *verticesAddress;
		verticesAddress++;
	}

	for (int x{ 0 }; x < Utils::m_sizeIndices; x++)
	{
		Indices[x] = *indicesAddress;
		indicesAddress++;
	}

	iSize = 8;
	
//	m_pCamera = &_camera;
	m_Camera = _camera;
	m_position = _position;

	if (_IsPlayer)
		m_PlayerPosition = _position;

	m_startingPosition = _position;
	m_numVertices = _numVertices;
	m_ModelType = _model;
//	glewInit();
	ShaderLoader shaderLoader;
	//	program = shaderLoader.CreateProgram("VAO_Triangle.vs", "VAO_Triangle.fs");
	program = shaderLoader.CreateProgram("Assets//Shaders//Shape.vs", "Assets//Shaders//Shape.fs");

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FILL, GL_LINE

	//** VAO
	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind the Vertex Array Object to openGl context
	glBindVertexArray(vao);//otherwise glVertexAttribPointer


	//** VBO
	// Then bind and set vertex buffer(s).
	// First paramter is how many buffers we have to create 
	glGenBuffers(1, &vbo);
	// bind VBO to binding point, here it is GL_ARRAY_BUFFER
	// there are other binding points
	glBindBuffer(GL_ARRAY_BUFFER, vbo);//bind to context

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(Vertices),// GPU need to know how much memory needs to be allocated
		Vertices,//copy data to GPU
		GL_STATIC_DRAW);// How to use the buffer - buffer is created and modified once


	//** EBO
	// GL Element Array Buffer
	//GL_ELEMENT_ARRAY_BUFFER 
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//**  Load and bind texture 1
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texutre wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//**Load image and create texture 1
	// Load image, create texture and generate mipmaps
	int width, height;

//	char* filename1{ 0 };

	//if (m_ModelType == QUAD)
	//	filename1 = "Dirt.jpg";

	//else if (m_ModelType == PYRAMID)
	//	filename1 = "Pattern.jpg";

	//else if (m_ModelType == CUBE)
	//{
	//	if (m_IsPlayer)
	//		filename1 = "PlayerTexture.jpg";
	//	else
	//		filename1 = "Assets//Textures//EnemyTexture.jpg";
	//}

	//else if (m_ModelType == DOT)

	//	if (m_bIsEnemyBullet)
	//		filename1 = "EnemyBullet.png";
	//	else
	//		filename1 = "PlayerBullet.png";

	//else if (m_ModelType == TRIANGLE)
	//{
	//	filename1 = "Bullet.png";
	//	iSize = 6;
	//}

	//else // Octagon
	//{
	//	filename1 = "White.jpg";
	//	iSize = 6;
	//}
		
	unsigned char* image = SOIL_load_image(m_pcFileName, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);




	//**  Load and bind texture 2
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// Set texutre wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//**Load image and create texture 2
	// Load image, create texture and generate mipmaps

	//char* filename2{ 0 };

	//if (m_ModelType == QUAD)
	//	filename2 = "Dirt.jpg";

	//else if (m_ModelType == PYRAMID)
	//	filename2 = "Pattern.jpg";

	//else if (m_ModelType == CUBE)
	//{
	//	if (m_IsPlayer)
	//		filename2 = "PlayerTexture.jpg";
	//	else
	//		filename2 = "Assets//Textures//EnemyTexture.jpg";
	//}

	//else if (m_ModelType == DOT)

	//	if (m_bIsEnemyBullet)
	//		filename2 = "EnemyBullet.png";
	//	else
	//		filename2 = "PlayerBullet.png";


	
	//else if (m_ModelType == TRIANGLE)
	//{
	//	filename2 = "Bullet.png";
	//	iSize = 6;
	//}

	//else // Octagon
	//{
	//	filename2 = "White.jpg";
	//	iSize = 6;
	//}
	

	unsigned char* image2 = SOIL_load_image(m_pcFileName, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image2);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attributes

	//** Vertex Attribute
	// Then at the end add attribute pointer(s)
	glVertexAttribPointer(0, //tie vertex attrib from shader to OpenGL
		3, // sending 3 vertices 
		GL_FLOAT, // type of data sent
		GL_FALSE, // is data normalized
		iSize * sizeof(GLfloat), // size of the data         // changed to 8 from 6
		(GLvoid*)0); // stride
	glEnableVertexAttribArray(0);

	//** Color Attribute
	glVertexAttribPointer(1, //tie vertex attrib from shader to OpenGL
		3, // sending 3 vertices 
		GL_FLOAT, // type of data sent
		GL_FALSE, // is data normalized
		iSize * sizeof(GLfloat), // size of the data       // changed to 8 from 6
		(GLvoid*)(3 * sizeof(GLfloat))); // stride
	glEnableVertexAttribArray(1);

	//** TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, iSize * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	// It's always a good thing to unbind any buffer/array to prevent strange bugs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

/***********************
* Model render: renders the model 
* Parameters: vec3 current velocity
* Return: void
********************/
void Model::Render(vec3 _CurrentVelocity)
{
	GLfloat currentTime = (GLfloat)(glutGet(GLUT_ELAPSED_TIME)); //get current time
	currentTime = currentTime / 1000; // convert millisecond to seconds 

	glBindBuffer(GL_ARRAY_BUFFER, vbo);//bind to context

	// draw first triangle
	glUseProgram(program);
	//glBindVertexArray(vao);

	// glDrawArrays(GL_TRIANGLE, 0, 3);

	// bind texture 1
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program, "Texture"), 0);

	// bind texture 2
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(program, "Texture2"), 1);

	glBindVertexArray(vao);
	GLint modelLoc = glGetUniformLocation(program, "model");

	glm::mat4 model;
	
	if (m_ModelType == CUBE)
	{
		m_position += _CurrentVelocity;

		if (m_IsPlayer)
		{
			m_position = CheckEdgeCollision();
			m_PlayerPosition = m_position; // static variable stored in all models so they know the player's current position
		}
		else //AI
		{
			/*if (HasEnteredMap())
				m_bOutsideMap = false;

			if (HasExitedMap())
				m_bOutsideMap = true;*/

		//	if (!m_bOutsideMap)
			//	m_position = CheckEdgeCollisionAI();

			if (m_IsLeader)
				m_LeaderPosition = m_position; // static variable stored in all models so they know the player's current position
		}

		model = glm::translate(model, m_position);
	}

	if (m_ModelType == DOT)
	{
		if (IsAtEdge())
		{
			m_bToBeDeleted = true;
		}

		else
			m_position += _CurrentVelocity;
			model = glm::translate(model, m_position);
	}

	else if (m_ModelType == PYRAMID)
	{
		
		m_position += _CurrentVelocity;

		m_position = CheckEdgeCollision();

		model = glm::translate(model, m_position);
		
		GLfloat rotangle = 15.0f;
		model = rotate(model, rotangle * abs((currentTime / 5)), glm::vec3(0.0f, -1.0f, 0.0f));	
	}

	//else if (m_ModelType == CUBE) // enemy
	//{
	//	/*float fSpeed = 5;

	//	printf("Move cube forward left \n");
	//	m_position += glm::vec3(-fSpeed, 0.0f, -fSpeed) * _fDeltaTick;*/
	//}

	////////////////////////////////////////
//	model = glm::translate(model, m_position);
	////////////////////////////////////////
	
	//else if (m_ModelType == PYRAMID)
	//{
	//	model = glm::translate(model, m_position);

	//	if (keyState[(unsigned char)'q'] == BUTTON_DOWN)
	//	{
	//		printf("Rotate pyramid left \n");
	//		GLfloat rotangle = 20.0f;
	//		model = rotate(model, rotangle * abs((currentTime / 5)), glm::vec3(0.0f, -1.0f, 0.0f));
	//	}

	//	else if (keyState[(unsigned char)'e'] == BUTTON_DOWN)
	//	{
	//		printf("Rotate pyramid right \n");
	//		GLfloat rotangle = 20.0f;
	//		model = rotate(model, rotangle * abs((currentTime / 5)), glm::vec3(0.0f, 1.0f, 0.0f));
	//	}
	//}

	//else if (m_ModelType == QUAD)
	//{
	//	model = glm::translate(model, m_position);
	//	GLfloat rotangle = 20.0f;
	//    model = rotate(model, rotangle * abs(sin(currentTime / 5)), glm::vec3(1.0f, 0.0f, 0.0f));
	//}

	//else if (m_ModelType == TRIANGLE)
	//{
	//	model = glm::translate(model, m_position);
	//	GLfloat rotangle = 10.0f;
	//    model = rotate(model, rotangle * abs(cos(currentTime / 5)), glm::vec3(0.0f, 0.0f, 1.0f));
	//}

	//else if (m_ModelType == OCTAGON)
	//{
	//	model = glm::translate(model, m_position);
	//	GLfloat rotangle = 10.0f;
	//	model = rotate(model, rotangle * abs((currentTime / 5)), glm::vec3(1.0f, -1.0f, 1.0f));
	//}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, m_numVertices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	///////////////////////////////////////////////
	// Update

	//These 2 lines laready included above
	//GLfloat currentTime = glutGet(GLUT_ELAPSED_TIME); //get current time
	//currentTime = currentTime / 1000; // convert millisecond to seconds 

	GLint currentTimeLocation = glGetUniformLocation(program, "currentTime"); // get uniform lcoation from program 
	glUniform1f(currentTimeLocation, currentTime); // set value
	// Create transformation matrices
//	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	view = m_Camera.GetView();
	projection = m_Camera.GetProjection();


	// Get their uniform location
	//	GLint modelLoc = glGetUniformLocation(program, "model");
	GLint viewLoc = glGetUniformLocation(program, "view");
	GLint projLoc = glGetUniformLocation(program, "projection");

	// Pass them to the shaders
	//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay(); //the render function is called

	////////////////////////////////////////////

//	glutSwapBuffers(); // Put above instead
}

ModelType Model::GetModelType()
{
	return m_ModelType;
}

vec3 Model::GetPosition()
{
	return m_position;
}

vec3 Model::GetPlayerPosition()
{
	return m_PlayerPosition;
}

vec3 Model::GetLeaderPosition()
{
	return m_LeaderPosition;
}

bool Model::IsLeader()
{
	if (m_IsLeader)
		return true;

	else
		return false;
}

/***********************
* Model check edge collision: checks the collisions between the outer walls of the map and the player and returns an updated position if needed
* Parameters: none
* Return: vec3 position
********************/
vec3 Model::CheckEdgeCollision()
{
	float fMapSize = MAP_SIZE - 0.5f;   // from utils function 

	vec3 Position = m_position;

	if (m_position.x > fMapSize)
		Position.x = fMapSize;

	else if (m_position.x < -fMapSize)
		Position.x = -fMapSize;

	if (m_position.z > fMapSize)
		Position.z = fMapSize;

	else if (m_position.z < -fMapSize)
		Position.z = -fMapSize;

	return Position;
}

/***********************
* Model is at edge: checks if a model is at the edge of the map
* Parameters: none
* Return: bool true or false
********************/
bool Model::IsAtEdge()
{	
	float fMapSize = 0.0f;
	
	if ((m_ModelType == CUBE) || (m_ModelType == PYRAMID))// for enemies wandering
		fMapSize = MAP_SIZE - 0.51f;   // slightly smaller than edge collision function above

	else if (m_ModelType == DOT) // for bullets
		fMapSize = MAP_SIZE - 0.5f;

	if ((m_position.x > fMapSize) || (m_position.x < -fMapSize) || (m_position.z > fMapSize) || (m_position.z < -fMapSize))
		return true;

	else
		return false;
}

/***********************
* Model has entered map: checks if an enemy has entered the map after spawning
* Parameters: none
* Return: bool true or false
********************/
bool Model::HasEnteredMap()
{
	float fMapSize = MAP_SIZE -0.55f;

	if (((m_position.x >= -fMapSize) && (m_position.x <= fMapSize)) &&
	    ((m_position.z >= -fMapSize) && (m_position.z <= fMapSize)))
		return true;

	else
		return false;
}

/***********************
* Model has exited map: checks if an enemy has exited the map
* Parameters: none
* Return: bool true or false
********************/
bool Model::HasExitedMap()
{
	float fMapSize = MAP_SIZE -0.55f;

	if (((m_position.x < -fMapSize) && (m_position.x > fMapSize)) &&
		((m_position.z < -fMapSize) && (m_position.z > fMapSize)))
		return true;

	else
		return false;
}

/***********************
* Model is within flocking distance: checks if an enemy is within a certain distance to the leader enemy
* Parameters: float distance
* Return: bool true or false
********************/
bool Model::IsWithinFlockingDistance(float _fDistance)
{
	if ((abs((m_position.x) - (m_LeaderPosition.x)) < _fDistance) &&   // within a distance
		(abs((m_position.z) - (m_LeaderPosition.z)) < _fDistance))
		return true;

	else
		return false;
}

/***********************
* Model is within player range: checks if a model is within a certain distance to the player
* Parameters: float distance
* Return: bool true or false
********************/
bool Model::IsWithinPlayerRange(float _fDistance)
{
	if ((abs((m_position.x) - (m_PlayerPosition.x)) < _fDistance) &&   // within a distance
		(abs((m_position.z) - (m_PlayerPosition.z)) < _fDistance))
		return true;

	else
		return false;
}

bool Model::ToDelete()
{
	return m_bToBeDeleted;
}

void Model::SetToBeDeleted()
{
	m_bToBeDeleted = true;
}

vec3 Model::GetLastPlayerVelocity()
{
	return m_LastPlayerVelocity;;
}

void Model::SetLastPlayerVelocity(vec3 _velocity)
{
	m_LastPlayerVelocity = _velocity;
}

void Model::SetLeader(bool _b)
{
	m_IsLeader = _b;
}

void Model::SetPosition(vec3 _position)
{
	m_position = _position;
}

void Model::ResetToBeDeleted()
{
	m_bToBeDeleted = false;
}

void Model::ResetToStartingPosition()
{
	m_position = m_startingPosition;
}

void Model::SetOutsideMap(bool _b)
{
	m_bOutsideMap = _b;
}

bool Model::IsOutsideMap()
{
	return m_bOutsideMap;
}

void Model::SetAsEnemyBullet()
{
	m_bIsEnemyBullet = true;
}



	

