#pragma once
#include "GameObject.h"
#include "Game.h"

struct Color
{
	float r;
	float g;
	float b;

	void SetColor(float r1, float g1, float b1) {
		r = r1;
		g = g1;
		b = b1;
	}
};

class PencilObject : public GameObject
{
public:
	PencilObject();
	PencilObject(Vector2 startPos, int objectID, int ownerID);
	~PencilObject();

	void update(float deltaTime);

	void render();

	void handleInput(SDL_Event event);

	void HandleNetworkData(int packedData);

private:
	void addPoints(Vector2 newPoint);
	void SetColorByPlayerID();

private:
	// very temporary variables, this is inteded to be a base class
	float mTimer;
	float mColorValue;

	Vector2 mLastPointAddedPosition;

	std::vector<Vector2> mPoints;
	bool mFinishedDrawing;

	Color mColor;
};

