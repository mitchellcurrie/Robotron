//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : ShaderLoader.h
// Description : Shader Loader header file - functions to read shader, create shader and create program
// Mail : Mitchell.Currie@mediadesignschool.com
//

#pragma once

#include "glew.h"
#include "freeglut.h"
#include "ShaderLoader.h"
#include <iostream>


class ShaderLoader
{
private:

	std::string ReadShader(char *filename);
	GLuint CreateShader(GLenum shaderType,
		std::string source,
		char* shaderName);

public:

	ShaderLoader(void);
	~ShaderLoader(void);
	GLuint CreateProgram(char* VertexShaderFilename,
		char* FragmentShaderFilename);

};
