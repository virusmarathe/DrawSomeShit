#pragma once
#include "GameObject.h"
#include "FontSheet.h"

class TextObject : public GameObject
{
public:
	TextObject(Vector2 pos, FontSheet * fontSheetRef, std::string text);
	~TextObject();

	void update(float deltaTime);

	void render();

	void handleInput(SDL_Event event);

private:
	FontSheet * mFontSheetRef;
	std::string mText;
	GLuint mTextureID;
	GLuint mVBOID;
	GLfloat mSpace;
	GLfloat mNewLine;
	std::vector<Rect> mClips;
	float mTimer;
};

