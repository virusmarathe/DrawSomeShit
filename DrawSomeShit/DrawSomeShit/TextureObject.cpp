#include "TextureObject.h"

TextureObject::TextureObject()
{
	mTextureID = 0;
	mTextureWidth = 0;
	mTextureHeight = 0;
}

TextureObject::TextureObject(Vector2 startPos, int objectID, int ownerID) :GameObject(startPos, objectID, ownerID)
{
	mTextureID = 0;
	mTextureWidth = 0;
	mTextureHeight = 0;
}


TextureObject::~TextureObject()
{
	freeTexture();
}

bool TextureObject::loadTextureFromPixels32(GLuint * pixels, GLuint width, GLuint height)
{
	freeTexture();

	mTextureWidth = width;
	mTextureHeight = height;

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, NULL);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error loading texture from %p pixels! %s\n", pixels, gluErrorString(error));
		return false;
	}

	return true;
}

void TextureObject::freeTexture()
{
	if (mTextureID != 0)
	{
		glDeleteTextures(1, &mTextureID);
		mTextureID = 0;
	}

	mTextureWidth = 0;
	mTextureHeight = 0;
}

void TextureObject::render()
{
	//If the texture exists
	if (mTextureID != 0)
	{
		//Remove any previous transformations
		glLoadIdentity();

		//Move to rendering point
		glTranslatef(mPosition.X, mPosition.Y, 0.f);

		glColor3f(1, 1, 1);
		//Set texture ID
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		//Render textured quad
		glBegin(GL_QUADS);
		glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
		glTexCoord2f(1.f, 0.f); glVertex2f(mTextureWidth, 0.f);
		glTexCoord2f(1.f, 1.f); glVertex2f(mTextureWidth, mTextureHeight);
		glTexCoord2f(0.f, 1.f); glVertex2f(0.f, mTextureHeight);
		glEnd();
	}
}
