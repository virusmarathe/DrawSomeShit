#pragma once
#include "GameObject.h"
class TextureObject : public GameObject
{
public:
	TextureObject();
	TextureObject(Vector2 startPos, int objectID, int ownerID);
	virtual ~TextureObject();

	bool loadTextureFromPixels32(GLuint * pixels, GLuint width, GLuint height);

	void freeTexture();

	virtual void render();

	virtual void update(float deltaTime) { }

	virtual void handleInput(SDL_Event event) { }

	GLuint getTextureID() { return mTextureID; }
	GLuint getTextureWidth() { return mTextureWidth; }
	GLuint getTextureHeight() { return mTextureHeight; }

private:

	GLuint mTextureID;
	GLuint mTextureWidth;
	GLuint mTextureHeight;
};

