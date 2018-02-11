#include "GameObject.h"
#include <iostream>


GameObject::GameObject()
{
	mTimer = 0.0f;
	mShapeScale = 0.0f;
}


GameObject::~GameObject()
{
}

void GameObject::update(float deltaTime)
{
	// dummy update logic for testing
	float fillScreenDuration = 4.0f; // takes 4 seconds to fill the screen
	mTimer += deltaTime;
	mShapeScale = mTimer / fillScreenDuration;
	if (mTimer > fillScreenDuration)
	{
		mTimer = 0;
	}
}

void GameObject::render()
{
	glColor3f(0, mShapeScale, 0.7f);
	// for now draws a bullshit quad in screen space
	glBegin(GL_QUADS);
	glVertex2f(-mShapeScale, -mShapeScale);
	glVertex2f(mShapeScale, -mShapeScale);
	glVertex2f(mShapeScale, mShapeScale);
	glVertex2f(-mShapeScale, mShapeScale);
	glEnd();
}
