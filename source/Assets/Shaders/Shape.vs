// Vertex shader

#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 outColor;
out vec2 TexCoord;

// uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float currentTime;
uniform int frameNumber;

void main (void)
{
//	gl_Position = vec4(position.x *cos(currentTime) - position.y *sin(currentTime) , position.x *sin(currentTime) + position.y *cos(currentTime) , position.z , 1.0);
//	gl_Position = vec4(position.x, position.y, position.z, 1.0);
//	gl_Position = vec4(position.x + sin(currentTime/2), position.y, position.z, 1.0);

//	gl_Position = transform * vec4(position, 1.0);

    gl_Position = projection * view * model * vec4(position, 1.0f);

	outColor = color;

	TexCoord = vec2(texCoord.x, texCoord.y);
}