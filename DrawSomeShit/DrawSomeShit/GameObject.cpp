#include "GameObject.h"
#include <iostream>
#include <math.h>

#define PI 3.14159265
#define DRAW_NEW_DISTANCE 5

GameObject::GameObject()
{
	mTimer = 0.0f;
	mColorValue = 0.0f;
	mPosition = Vector2(0, 0);
}

GameObject::GameObject(Vector2 startPos)
{
	mTimer = 0.0f;
	mColorValue = 0.0f;
	mPosition = startPos;
	AddPoints(startPos);
	mLastPointAddedPosition = Vector2(0, 0);
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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// move object to position, relavent for models, not so much for drawing
	//glTranslatef(mPosition.X, mPosition.Y, 0.0f);

	glColor3f(cos(mColorValue), sin(mColorValue), 0.7f);

	// for now draws a bullshit quad in screen space
	// eventually will read through model data (position, color, normal)
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < mPoints.size(); i++)
	{
		glVertex2f(mPoints[i].X, mPoints[i].Y);
	}
	glEnd();
}

void GameObject::AddPoints(Vector2 newPoint)
{
	if (mLastPointAddedPosition.distSqr(newPoint) > DRAW_NEW_DISTANCE * DRAW_NEW_DISTANCE)
	{
		mPoints.push_back(newPoint);
		mLastPointAddedPosition = newPoint;
	}
}
