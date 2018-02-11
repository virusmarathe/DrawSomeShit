#pragma once

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

	virtual void update(float deltaTime);

	virtual void render();

	void SetPosition(Vector2 pos) { mPosition = pos; }

	void AddPoints(Vector2 newPoint);

private:

	// very temporary variables, this is inteded to be a base class
	float mTimer;
	float mColorValue;

	Vector2 mPosition;
	Vector2 mLastPointAddedPosition;

	std::vector<Vector2> mPoints;
};

