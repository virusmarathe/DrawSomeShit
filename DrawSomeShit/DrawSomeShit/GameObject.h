#pragma once

#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void update(float deltaTime);

	virtual void render();

private:

	// very temporary variables, this is inteded to be a base class
	float mTimer;
	float mShapeScale;
};

