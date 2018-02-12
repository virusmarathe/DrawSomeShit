#pragma once

#include "SDL.h"
#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Vector2.h"
#include <vector>

class GameObject
{
public:
	GameObject();
	GameObject(Vector2 startPos);
	~GameObject();

	virtual void update(float deltaTime) = 0;

	virtual void render() = 0;

	virtual void handleInput(SDL_Event event) = 0;

	void setPosition(Vector2 pos) { mPosition = pos; }


protected:
	Vector2 mPosition;
};

