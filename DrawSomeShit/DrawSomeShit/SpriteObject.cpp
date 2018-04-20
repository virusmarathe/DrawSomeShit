#include "SpriteObject.h"



SpriteObject::SpriteObject(Vector2 pos, GLuint textureID, GLuint VBOID, GLuint IBOID)
{
	mPosition = pos;
	mTextureID = textureID;
	mVBOID = VBOID;
	mIBOID = IBOID;
}


SpriteObject::~SpriteObject()
{
}

void SpriteObject::update(float deltaTime)
{
}

void SpriteObject::render()
{
	glLoadIdentity();

	//Move to rendering point
	glTranslatef(mPosition.X, mPosition.Y, 0.f);
	glColor3f(1, 1, 1);

	if (mVBOID != NULL)
	{
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			
		glBindBuffer(GL_ARRAY_BUFFER, mVBOID);
		glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData2D), (GLvoid*)offsetof(VertexData2D, texCoord));
		glVertexPointer(2, GL_FLOAT, sizeof(VertexData2D), (GLvoid*)offsetof(VertexData2D, position));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBOID);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void SpriteObject::handleInput(SDL_Event event)
{
}
