#pragma once

#include "glew.h"
#include "freeglut.h"
#include "SOIL.h"
#include "utils.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

#include "ShaderLoader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class TextLabel
{
public:
	TextLabel(TextType _type, std::string text, std::string font);
	~TextLabel();

	void Render();
	void setPosition(glm::vec2 _position);
	void setColor(glm::vec3 _color);
	void setScale(GLfloat _scale);
	void setText(std::string _text);
	TextType GetTextType();
	glm::vec2 GetPosition() const;

private:
	std::string text;
	GLfloat scale;
	glm::vec3 color;
	glm::vec2 position;

	GLuint VAO, VBO, program;
	std::map<GLchar, Character> Characters;
	TextType m_TextType;

};

