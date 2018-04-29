#include "TextObject.h"



TextObject::TextObject(Vector2 pos, FontSheet * fontSheetRef, std::string text)
{
	mPosition = pos;
	mFontSheetRef = fontSheetRef;
	mText = text;
	mVBOID = mFontSheetRef->getVertexDataBuffer();
	mSpace = mFontSheetRef->getSpace();
	mNewLine = mFontSheetRef->getNewLine();
	mTextureID = mFontSheetRef->getTextureID();
	mClips = mFontSheetRef->getClipsRef();
	mTimer = 0.0f;
}


TextObject::~TextObject()
{
	mFontSheetRef = NULL;
}

void TextObject::update(float deltaTime)
{
	mTimer += deltaTime;

	if (mTimer > 10.0f)
	{
		mText = "Huuto is the worst!";
	}
}

void TextObject::render()
{
	GLfloat drawX = mPosition.X;
	GLfloat drawY = mPosition.Y;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (mTextureID != 0)
	{
		glTranslatef(mPosition.X, mPosition.Y, 0.0f);
		glColor3f(1, 1, 1);

		glBindTexture(GL_TEXTURE_2D, mTextureID);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, mVBOID);
		glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData2D), (GLvoid*)offsetof(VertexData2D, texCoord));
		glVertexPointer(2, GL_FLOAT, sizeof(VertexData2D), (GLvoid*)offsetof(VertexData2D, position));

		for (int i = 0; i < mText.length(); i++)
		{
			if (mText[i] == ' ')
			{
				glTranslatef(mSpace, 0.0f, 0.0f);
				drawX += mSpace;
			}
			else if (mText[i] == '\n')
			{
				glTranslatef(mPosition.X - drawX, mNewLine, 0.0f);
				drawY += mNewLine;
				drawX += mPosition.X - drawX;
			}
			else
			{
				GLuint ascii = (unsigned char)mText[i];

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFontSheetRef->getIndexBufferAtIndex(ascii));
				glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

				glTranslatef(mClips[ascii].w, 0.0f, 0.0f);
				drawX += mClips[ascii].w;
			}
		}

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindTexture(GL_TEXTURE_2D, NULL);
	}
}

void TextObject::handleInput(SDL_Event event)
{
}