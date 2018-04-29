#include "FontSheet.h"



FontSheet::FontSheet()
{
	mSpace = 0.0f;
	mLineHeight = 0.0f;
	mNewLine = 0.0f;
}


FontSheet::~FontSheet()
{
	freeFont();
}

bool FontSheet::loadBitmap(std::string path)
{
	bool success = true;

	const GLuint BLACK_PIXEL = 0xFF000000;
	const GLfloat GRID_SIZE = 16.0f;

	freeFont();

	if (loadPixelsFromFile(path))
	{
		GLfloat cellW = getTextureWidth() / GRID_SIZE;
		GLfloat cellH = getTextureHeight() / GRID_SIZE;

		GLuint top = cellH;
		GLuint bottom = 0;
		GLuint aBottom = 0;

		int pixelX = 0;
		int pixelY = 0;

		int cellX = 0;
		int cellY = 0;

		float charPadding = 5.0f;

		GLuint currentChar = 0;
		Rect nextClip = { 0.0f, 0.0f, cellW, cellH };

		for (unsigned int rows = 0; rows < GRID_SIZE; rows++)
		{
			for (unsigned int cols = 0; cols < GRID_SIZE; cols++)
			{
				cellX = cellW * cols;
				cellY = cellH * rows;

				nextClip.x = cellX;
				nextClip.y = cellY;
				nextClip.w = cellW;
				nextClip.h = cellH;

				// set left side of current character (clip.x)
				for (int pixelCol = 0; pixelCol < cellW; ++pixelCol)
				{
					for (int pixelRow = 0; pixelRow < cellH; ++pixelRow)
					{
						pixelX = cellX + pixelCol;
						pixelY = cellY + pixelRow;

						if (getPixel32(pixelX, pixelY) != BLACK_PIXEL)
						{
							nextClip.x = pixelX;
							pixelCol = cellW;
							pixelRow = cellH;
						}
					}
				}

				// set width of current character (clip.w)
				for (int pixelCol = cellW - 1; pixelCol >= 0; --pixelCol)
				{
					for (int pixelRow = 0; pixelRow < cellH; ++pixelRow)
					{
						pixelX = cellX + pixelCol;
						pixelY = cellY + pixelRow;

						if (getPixel32(pixelX, pixelY) != BLACK_PIXEL)
						{
							nextClip.w = (pixelX - nextClip.x) + 1 + charPadding;
							pixelCol = -1;
							pixelRow = cellH;
						}
					}
				}

				// find highest top of all character (clip.y eventually)
				for (int pixelRow = 0; pixelRow < cellH; ++pixelRow)
				{
					for (int pixelCol = 0; pixelCol < cellW; ++pixelCol)
					{
						pixelX = cellX + pixelCol;
						pixelY = cellY + pixelRow;

						if (getPixel32(pixelX, pixelY) != BLACK_PIXEL)
						{
							if (pixelRow < top)
							{
								top = pixelRow;
							}

							pixelCol = cellW;
							pixelRow = cellH;
						}
					}
				}

				// find bottom of characters
				for (int pixelRow = cellH - 1; pixelRow >= 0; --pixelRow)
				{
					for (int pixelCol = 0; pixelCol < cellW; ++pixelCol)
					{
						pixelX = cellX + pixelCol;
						pixelY = cellY + pixelRow;

						if (getPixel32(pixelX, pixelY) != BLACK_PIXEL)
						{
							if (currentChar == 'A')
							{
								aBottom = pixelRow;
							}
							if (pixelRow > bottom)
							{
								bottom = pixelRow;
							}

							pixelCol = cellW;
							pixelRow = -1;
						}
					}
				}

				mClips.push_back(nextClip);
				currentChar++;
			}
		}

		for (int t = 0; t < (GRID_SIZE * GRID_SIZE); t++)
		{
			mClips[t].y += top;
			mClips[t].h -= top;
		}

		const int PIXEL_COUNT = getTextureWidth() * getTextureHeight();
		GLuint* pixels = getPixelData32();
		for (int i = 0; i < PIXEL_COUNT; i++)
		{
			GLubyte* colors = (GLubyte*)&pixels[i];
			colors[3] = colors[0]; // set alpha to red channel brightness for smoother blending
			colors[0] = 0xFF;
			colors[1] = 0xFF;
			colors[2] = 0xFF;
		}

		if (loadTextureFromPixels32())
		{
			if (!generateDataBuffer(SPRITE_ORIGIN_TOP_LEFT))
			{
				std::cout << "Failed to create vertex buffer for bitmap font!" << std::endl;
				success = false;
			}
		}
		else
		{
			std::cout << "Failed to create texture from bitmap font pixels!" << std::endl;
			success = false;
		}

		glBindTexture(GL_TEXTURE_2D, getTextureID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		mSpace = cellW / 3.0f;
		mNewLine = aBottom - top;
		mLineHeight = bottom - top;

		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	else
	{
		std::cout << "Failed to load bitmap font image: " << path.c_str() << std::endl;
		success = false;
	}

	return success;
}

void FontSheet::freeFont()
{
	freeTexture();

	mSpace = 0.0f;
	mLineHeight = 0.0f;
	mNewLine = 0.0f;
}
