#include "GameObject.h"
#include <iostream>
#include <math.h>

GameObject::GameObject()
{
	mPosition = Vector2(0, 0);
	mOwnerID = -1;
}

GameObject::GameObject(Vector2 startPos, int objectID, int ownerID)
{
	mPosition = startPos;
	mID = objectID;
	mOwnerID = ownerID;
}


GameObject::~GameObject()
{
}

void GameObject::HandleNetworkData(int packedData)
{
}
