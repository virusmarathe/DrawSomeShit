#include "GameObject.h"
#include <iostream>
#include <math.h>

GameObject::GameObject()
{
	mPosition = Vector2(0, 0);
}

GameObject::GameObject(Vector2 startPos)
{
	mPosition = startPos;
}


GameObject::~GameObject()
{
}

void GameObject::HandleNetworkData(int packedData)
{
}
