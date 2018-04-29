#pragma once
#include "SpriteSheet.h"
class FontSheet : public SpriteSheet
{
public:
	FontSheet();
	~FontSheet();

	bool loadBitmap(std::string path);

	void freeFont();

	GLfloat getSpace() { return mSpace; }
	GLfloat getLineHeight() { return mLineHeight; }
	GLfloat getNewLine() { return mNewLine; }

private:
	GLfloat mSpace;
	GLfloat mLineHeight;
	GLfloat mNewLine;
};

