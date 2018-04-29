#include "TextureObject.h"

TextureObject::TextureObject()
{
	mTextureID = 0;
	mTextureWidth = 0;
	mTextureHeight = 0;
	mClipRect = NULL;
	mVBOID = 0;
	mIBOID = 0;
	mPixels = NULL;
}

TextureObject::TextureObject(Vector2 startPos, Rect * clipRect, int objectID, int ownerID) :GameObject(startPos, objectID, ownerID)
{
	mTextureID = 0;
	mTextureWidth = 0;
	mTextureHeight = 0;
	mClipRect = clipRect;
	mVBOID = 0;
	mIBOID = 0;
	mPixels = NULL;
}


TextureObject::~TextureObject()
{
	freeTexture();

	freeVBO();
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

	initVBO();

	return true;
}

bool TextureObject::loadTextureFromPixels32()
{
	bool success = true;

	if (mTextureID == 0 && mPixels != NULL)
	{
		glGenTextures(1, &mTextureID);
		glBindTexture(GL_TEXTURE_2D, mTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mPixels);

		//Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, NULL);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			printf("Error loading texture from %p pixels! %s\n", mPixels, gluErrorString(error));
			success = false;
		}
		else
		{
			//Release pixels
			delete[] mPixels;
			mPixels = NULL;

			//Generate VBO
			initVBO();
		}
	}
	else
	{
		printf("Cannot load texture from current pixels! ");
		if (mTextureID != 0)
		{
			printf("A texture is already loaded!\n");
		}
		else if (mPixels == NULL)
		{
			printf("No pixels to create texture from!\n");
		}
		success = false;
	}

	return success;
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

bool TextureObject::loadPixelsFromFile(std::string path)
{
	//Deallocate texture data
	freeTexture();

	bool pixelsLoaded = false;

	ILuint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);

	//Load image
	ILboolean success = ilLoadImage(path.c_str());

	if (success == IL_TRUE)
	{
		//Convert image to RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (success == IL_TRUE)
		{
			//Initialize dimensions
			mTextureWidth = (GLuint)ilGetInteger(IL_IMAGE_WIDTH);
			mTextureHeight = (GLuint)ilGetInteger(IL_IMAGE_HEIGHT);

			//Allocate memory for texture data
			GLuint size = mTextureWidth * mTextureHeight;
			mPixels = new GLuint[size];

			//Copy pixels
			memcpy(mPixels, ilGetData(), size * 4);
			pixelsLoaded = true;
		}

		ilDeleteImages(1, &imgID);
	}

	if (!pixelsLoaded)
	{
		printf("Unable to load %s\n", path.c_str());
	}

	return pixelsLoaded;
}

void TextureObject::freeTexture()
{
	if (mTextureID != 0)
	{
		glDeleteTextures(1, &mTextureID);
		mTextureID = 0;
	}

	if (mPixels != NULL)
	{
		delete[] mPixels;
		mPixels = NULL;
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

		VertexData2D vData[4];
		vData[0].texCoord.s = texLeft;	vData[0].texCoord.t = texTop;
		vData[1].texCoord.s = texRight; vData[1].texCoord.t = texTop;
		vData[2].texCoord.s = texRight; vData[2].texCoord.t = texBottom;
		vData[3].texCoord.s = texLeft;	vData[3].texCoord.t = texBottom;

		vData[0].position.x = 0.0f;			vData[0].position.y = 0.0f;
		vData[1].position.x = clipWidth;	vData[1].position.y = 0.0f;
		vData[2].position.x = clipWidth;	vData[2].position.y = clipHeight;
		vData[3].position.x = 0.0f;			vData[3].position.y = clipHeight;

		//Set texture ID
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			// bind and update vbo data
			glBindBuffer(GL_ARRAY_BUFFER, mVBOID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(VertexData2D), vData);
			// set texture data
			glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData2D), (GLvoid*)offsetof(VertexData2D, texCoord));
			// set vertex data
			glVertexPointer(2, GL_FLOAT, sizeof(VertexData2D), (GLvoid*)offsetof(VertexData2D, position));

			// bind IBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBOID);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void TextureObject::initVBO()
{
	if (mTextureID != 0 && mVBOID == 0)
	{
		VertexData2D vData[4];
		GLuint iData[4] = { 0, 1, 2, 3 };

		// create VBO
		glGenBuffers(1, &mVBOID);
		glBindBuffer(GL_ARRAY_BUFFER, mVBOID);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexData2D), vData, GL_DYNAMIC_DRAW);

		// create IBO
		glGenBuffers(1, &mIBOID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBOID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), iData, GL_DYNAMIC_DRAW);

		// unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
}

void TextureObject::freeVBO()
{
	if (mVBOID != 0)
	{
		glDeleteBuffers(1, &mVBOID);
		glDeleteBuffers(1, &mIBOID);
	}
}
