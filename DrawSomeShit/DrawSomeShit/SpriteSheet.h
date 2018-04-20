#pragma once
#include "TextureObject.h"
class SpriteSheet :	public TextureObject
{
public:
	SpriteSheet();
	~SpriteSheet();

	int addClipSprite(Rect& newClip);

	Rect getClip(int index);

	bool generateDataBuffer();

	void freeSheet();

	void freeTexture();

	GLuint getVertexDataBuffer() { return mVertexDataBuffer; }
	GLuint getIndexBufferAtIndex(int index) { return mIndexBuffers[index]; }

protected:
	std::vector<Rect> mClips;

	GLuint mVertexDataBuffer;
	GLuint* mIndexBuffers;
};

