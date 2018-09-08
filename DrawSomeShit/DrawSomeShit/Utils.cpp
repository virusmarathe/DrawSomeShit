#include "Utils.h"

#define MAX_OBJECTS 10000

int Utils::mIDCounter = 0;

Color Utils::GetColorForPlayer(int playerID)
{
	Color mColor;

	switch (playerID)
	{
	case 0:
		mColor.SetColor(1, 0, 0);
		break;
	case 1:
		mColor.SetColor(0, 1, 0);
		break;
	case 2:
		mColor.SetColor(0, 0, 1);
		break;
	case 3:
		mColor.SetColor(1, 1, 0);
		break;
	case 4:
		mColor.SetColor(0, 1, 1);
		break;
	case 5:
		mColor.SetColor(1, 0, 1);
		break;
	case 6:
		mColor.SetColor(1, 1, 1);
		break;
	case 7:
		mColor.SetColor(0.4f, 1, .25f);
		break;
	default:
		mColor.SetColor(1, 1, 1);
		break;
	}

	return mColor;
}

int Utils::GetNextObjectIDForPlayer(int playerID)
{
	int id = playerID * MAX_OBJECTS + Utils::mIDCounter;
	Utils::mIDCounter++;
	if (Utils::mIDCounter >= MAX_OBJECTS)
	{
		Utils::mIDCounter = 0;
	}
	return id;
}

void Utils::ToLowerCase(std::string &s)
{
	std::locale loc;

	for (std::string::size_type i = 0; i < s.length(); i++)
	{
		s[i] = std::tolower(s[i], loc);
	}
}
