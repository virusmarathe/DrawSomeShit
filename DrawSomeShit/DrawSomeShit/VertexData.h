#pragma once

#include <SDL_opengl.h>

struct VertexPos2D
{
	GLfloat x;
	GLfloat y;
};

struct TexCoord
{
	GLfloat s;
	GLfloat t;
};

struct VertexData2D
{
	VertexPos2D position;
	TexCoord texCoord;
};