#pragma once

#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Vector2.h"

class GameObject
{
public:
	GameObject();
	GameObject(Vector2 startPos);
	~GameObject();

	virtual void update(float deltaTime);

	virtual void render();

private:

	// very temporary variables, this is inteded to be a base class
	float mTimer;
	float mColorValue;

	Vector2 mPosition;
};

