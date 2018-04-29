#include "SpriteSheet.h"



SpriteSheet::SpriteSheet()
{
	mVertexDataBuffer = NULL;
	mIndexBuffers = NULL;
}


SpriteSheet::~SpriteSheet()
{
	freeSheet();
}

int SpriteSheet::addClipSprite(Rect & newClip)
{
	mClips.push_back(newClip);
	return mClips.size() - 1;
}

Rect SpriteSheet::getClip(int index)
{
	return mClips[index];
}

bool SpriteSheet::generateDataBuffer(SpriteOrigin origin)
{
	if (getTextureID() != 0 && mClips.size() > 0)
	{
		int numTotalSprites = mClips.size();
		VertexData2D * vertexData = new VertexData2D[numTotalSprites * 4];
		mIndexBuffers = new GLuint[numTotalSprites];

		glGenBuffers(1, &mVertexDataBuffer);
		glGenBuffers(numTotalSprites, mIndexBuffers);

		GLfloat texWidth = getTextureWidth();
		GLfloat texHeight = getTextureHeight();
		GLuint spriteIndices[4] = { 0, 0, 0 ,0 };

		GLfloat vTop = 0.0f;
		GLfloat vBottom = 0.0f;
		GLfloat vLeft = 0.0f;
		GLfloat vRight = 0.0f;

		for (int i = 0; i < numTotalSprites; i++)
		{
			spriteIndices[0] = i * 4 + 0;
			spriteIndices[1] = i * 4 + 1;
			spriteIndices[2] = i * 4 + 2;
			spriteIndices[3] = i * 4 + 3;

			switch (origin)
			{
			case SPRITE_ORIGIN_BOTTOM_LEFT:
				vTop = -mClips[i].h;
				vRight = mClips[i].w;
				break;
			case SPRITE_ORIGIN_BOTTOM_RIGHT:
				vTop = -mClips[i].h;
				vLeft = -mClips[i].w;
				break;
			case SPRITE_ORIGIN_TOP_LEFT:
				vBottom = mClips[i].h;
				vRight = mClips[i].w;
				break;
			case SPRITE_ORIGIN_TOP_RIGHT:
				vBottom = mClips[i].h;
				vLeft = -mClips[i].w;
				break;
			default:
				vTop = -mClips[i].h / 2.0f;
				vBottom = mClips[i].h / 2.0f;
				vLeft = -mClips[i].w / 2.0f;
				vRight = mClips[i].w / 2.0f;
				break;
			}

			// top left vertex
			vertexData[spriteIndices[0]].position.x = vLeft;
			vertexData[spriteIndices[0]].position.y = vTop;
			vertexData[spriteIndices[0]].texCoord.s = mClips[i].x / texWidth;
			vertexData[spriteIndices[0]].texCoord.t = mClips[i].y / texHeight;

			// top right vertex
			vertexData[spriteIndices[1]].position.x = vRight;
			vertexData[spriteIndices[1]].position.y = vTop;
			vertexData[spriteIndices[1]].texCoord.s = (mClips[i].x + mClips[i].w) / texWidth;
			vertexData[spriteIndices[1]].texCoord.t = mClips[i].y / texHeight;

			// bottom right vertex
			vertexData[spriteIndices[2]].position.x = vRight;
			vertexData[spriteIndices[2]].position.y = vBottom;
			vertexData[spriteIndices[2]].texCoord.s = (mClips[i].x + mClips[i].w) / texWidth;
			vertexData[spriteIndices[2]].texCoord.t = (mClips[i].y + mClips[i].h) / texHeight;

			// bottom left vertex
			vertexData[spriteIndices[3]].position.x = vLeft;
			vertexData[spriteIndices[3]].position.y = vBottom;
			vertexData[spriteIndices[3]].texCoord.s = mClips[i].x / texWidth;
			vertexData[spriteIndices[3]].texCoord.t = (mClips[i].y + mClips[i].h) / texHeight;

			// index data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffers[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), spriteIndices, GL_STATIC_DRAW);
		}

		// vertex data
		glBindBuffer(GL_ARRAY_BUFFER, mVertexDataBuffer);
		glBufferData(GL_ARRAY_BUFFER, numTotalSprites * 4 * sizeof(VertexData2D), vertexData, GL_STATIC_DRAW);

		delete[] vertexData;
	}
	else
	{
		if (getTextureID() == 0)
		{
			std::cout << "No texture to render with!" << std::endl;
		}
		if (mClips.size() <= 0)
		{
			std::cout << "No clipping areas to  generate vertex data from!" << std::endl;
		}
		return false;
	}
	return true;
}

void SpriteSheet::freeSheet()
{
	if (mVertexDataBuffer != NULL)
	{
		glDeleteBuffers(1, &mVertexDataBuffer);
		mVertexDataBuffer = NULL;
	}
	if (mIndexBuffers != NULL)
	{
		glDeleteBuffers(mClips.size(), mIndexBuffers);
		delete[] mIndexBuffers;
		mIndexBuffers = NULL;
	}

	mClips.clear();
}

void SpriteSheet::freeTexture()
{
	// get rid of sheet before calling base free
	freeSheet();

	TextureObject::freeTexture();
}
