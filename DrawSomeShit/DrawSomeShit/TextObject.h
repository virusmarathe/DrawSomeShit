#pragma once
#include "GameObject.h"
#include "FontSheet.h"
#include "Utils.h"

class TextObject : public GameObject
{
public:
	TextObject(Vector2 pos, int objectID, int ownerID, FontSheet * fontSheetRef, std::string text);
	~TextObject();

	void update(float deltaTime);

	void render();

	void handleInput(SDL_Event event);

	std::string getText() { return mText; }
	void setText(std::string text) { mText = text; }

	void forceUp();

	void setSent(bool val) { mIsSent = val; }

private:
	FontSheet * mFontSheetRef;
	std::string mText;
	GLuint mTextureID;
	GLuint mVBOID;
	GLfloat mSpace;
	GLfloat mNewLine;
	std::vector<Rect> mClips;
	bool mIsSent;
	Color mColor;
};

