#pragma once
#include "GameObject.h"
#include "VertexData.h"

class SpriteObject : public GameObject
{
public:
	SpriteObject(Vector2 pos, GLuint textureID, GLuint VBOID, GLuint IBOID);
	~SpriteObject();

	void update(float deltaTime);

	void render();

	void handleInput(SDL_Event event);

private:
	GLuint mTextureID;
	GLuint mVBOID;
	GLuint mIBOID;
};

