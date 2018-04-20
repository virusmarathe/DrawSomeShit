#pragma once
#include "GameObject.h"
#include <IL/il.h>
#include "VertexData.h"

struct Rect
{
	GLfloat x;
	GLfloat y;
	GLfloat w;
	GLfloat h;
};

class TextureObject : public GameObject
{
public:
	TextureObject();
	TextureObject(Vector2 startPos, Rect * clipRect, int objectID, int ownerID);
	virtual ~TextureObject();

	bool loadTextureFromPixels32(GLuint * pixels, GLuint width, GLuint height);
	bool loadTextureFromFile(std::string path);

	virtual void freeTexture();

	virtual void render();

	virtual void update(float deltaTime) { }

	virtual void handleInput(SDL_Event event) { }

	GLuint getTextureID() { return mTextureID; }
	GLuint getTextureWidth() { return mTextureWidth; }
	GLuint getTextureHeight() { return mTextureHeight; }

private:
	void initVBO();
	void freeVBO();

private:

	GLuint mTextureID;
	GLuint mTextureWidth;
	GLuint mTextureHeight;
	Rect * mClipRect;
	
	// VBO IDs
	GLuint mVBOID;
	GLuint mIBOID;
};

