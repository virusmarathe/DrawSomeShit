#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Utils.h"

class PencilObject : public GameObject
{
public:
	PencilObject();
	PencilObject(Vector2 startPos, int objectID, int ownerID);
	virtual ~PencilObject();

	void update(float deltaTime);

	void render();

	void handleInput(SDL_Event event);

	void HandleNetworkData(int packedData);

private:
	void addPoints(Vector2 newPoint);

private:
	// very temporary variables, this is inteded to be a base class
	float mTimer;
	float mColorValue;

	Vector2 mLastPointAddedPosition;

	std::vector<Vector2> mPoints;
	bool mFinishedDrawing;

	Color mColor;
};

