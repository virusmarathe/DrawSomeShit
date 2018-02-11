#include "GameObject.h"
#include <iostream>
#include <math.h>

#define PI 3.14159265

GameObject::GameObject()
{
	mTimer = 0.0f;
	mColorValue = 0.0f;
	mPosition = Vector2(0, 0);
}

GameObject::GameObject(Vector2 startPos)
{
	mPosition = startPos;
}


GameObject::~GameObject()
{
}

void GameObject::update(float deltaTime)
{
	// dummy update logic for testing
	float cycleDuration = 4.0f; // takes 4 seconds to fill the screen
	mTimer += deltaTime;
	if (mTimer > cycleDuration)
	{
		mTimer = mTimer - cycleDuration;
	}

	mColorValue = (2* PI) * (mTimer / cycleDuration);
}

void GameObject::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// move object to position
	glTranslatef(mPosition.X, mPosition.Y, 0.0f);

	glColor3f(cos(mColorValue), sin(mColorValue), 0.7f);

	// for now draws a bullshit quad in screen space
	// eventually will read through model data (position, color, normal)
	glBegin(GL_QUADS);
	glVertex2f(-50.0f, -50.0f);
	glVertex2f(50.0f, -50.0f);
	glVertex2f(50.0f, 50.0f);
	glVertex2f(-50.0f, 50.0f);
	glEnd();
}
