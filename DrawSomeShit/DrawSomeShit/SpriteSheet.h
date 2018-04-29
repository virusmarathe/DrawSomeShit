#pragma once
#include "TextureObject.h"

enum SpriteOrigin
{
	SPRITE_ORIGIN_CENTER,
	SPRITE_ORIGIN_TOP_LEFT,
	SPRITE_ORIGIN_BOTTOM_LEFT,
	SPRITE_ORIGIN_TOP_RIGHT,
	SPRITE_ORIGIN_BOTTOM_RIGHT
};

class SpriteSheet :	public TextureObject
{
public:
	SpriteSheet();
	~SpriteSheet();

	int addClipSprite(Rect& newClip);

	Rect getClip(int index);

	bool generateDataBuffer(SpriteOrigin origin = SPRITE_ORIGIN_CENTER);

	void freeSheet();

	void freeTexture();

	GLuint getVertexDataBuffer() { return mVertexDataBuffer; }
	GLuint getIndexBufferAtIndex(int index) { return mIndexBuffers[index]; }
	std::vector<Rect> getClipsRef() { return mClips; }

protected:
	std::vector<Rect> mClips;

	GLuint mVertexDataBuffer;
	GLuint* mIndexBuffers;
};

