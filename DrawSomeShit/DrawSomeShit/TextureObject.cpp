#include "TextureObject.h"

TextureObject::TextureObject()
{
	mTextureID = 0;
	mTextureWidth = 0;
	mTextureHeight = 0;
}

TextureObject::TextureObject(Vector2 startPos, Rect * clipRect, int objectID, int ownerID) :GameObject(startPos, objectID, ownerID)
{
	mTextureID = 0;
	mTextureWidth = 0;
	mTextureHeight = 0;
	mClipRect = clipRect;
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

bool TextureObject::loadTextureFromFile(std::string path)
{
	bool textureLoaded = false;

	ILuint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);

	ILboolean success = ilLoadImage(path.c_str());

	if (success == IL_TRUE)
	{
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (success == IL_TRUE)
		{
			textureLoaded = loadTextureFromPixels32((GLuint*)ilGetData(), (GLuint)ilGetInteger(IL_IMAGE_WIDTH), (GLuint)ilGetInteger(IL_IMAGE_HEIGHT));
		}

		ilDeleteImages(1, &imgID);
	}

	if (!textureLoaded)
	{
		std::cout << "Unable to load " << path.c_str() << "!";
	}

	return textureLoaded;
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

		GLfloat texTop = 0.0f;
		GLfloat texBottom = 1.0f;
		GLfloat texLeft = 0.0f;
		GLfloat texRight = 1.0f;
		GLfloat clipWidth = (GLfloat)mTextureWidth;
		GLfloat clipHeight = (GLfloat)mTextureHeight;

		// set values based on clip
		if (mClipRect != NULL)
		{
			texLeft = mClipRect->x / mTextureWidth;
			texRight = (mClipRect->x + mClipRect->w) / mTextureWidth;
			texTop = mClipRect->y / mTextureHeight;
			texBottom = (mClipRect->y + mClipRect->h) / mTextureHeight;

			clipWidth = mClipRect->w;
			clipHeight = mClipRect->h;
		}

		//Move to rendering point
		glTranslatef(mPosition.X, mPosition.Y, 0.f);

		glColor3f(1, 1, 1);
		//Set texture ID
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		//Render textured quad
		glBegin(GL_QUADS);
		glTexCoord2f(texLeft, texTop); glVertex2f(0.0f, 0.0f);
		glTexCoord2f(texRight, texTop); glVertex2f(clipWidth, 0.0f);
		glTexCoord2f(texRight, texBottom); glVertex2f(clipWidth, clipHeight);
		glTexCoord2f(texLeft, texBottom); glVertex2f(0.0f, clipHeight);
		glEnd();
	}
}
