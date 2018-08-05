#pragma once

#include "SDL.h"
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Vector2.h"
#include <vector>
#include "NetworkManager.h"

enum ObjectNetworkMessageType
{
	CREATE,
	UPDATE,
	FINISH,
	REMOVE,
	TEXT,
	GAMESTATECHANGE,
	NEXTDRAWERID,
	NEXTWORD
};

class GameObject
{
public:
	GameObject();
	GameObject(Vector2 startPos, int objectID, int ownerID);
	virtual ~GameObject();

	virtual void update(float deltaTime) = 0;

	virtual void render() = 0;

	virtual void handleInput(SDL_Event event) = 0;

	void setPosition(Vector2 pos) { mPosition = pos; }

	virtual void HandleNetworkData(int packedData);

	void SetID(int id) { mID = id; }
	int GetID() { return mID; }

	void SetOwner(int id) { mOwnerID = id; }

protected:
	Vector2 mPosition;
	int mID;
	int mOwnerID;
};

