#pragma once
#include <string>
#include <locale>

struct Color
{
	float r;
	float g;
	float b;

	void SetColor(float r1, float g1, float b1) {
		r = r1;
		g = g1;
		b = b1;
	}
};

class Utils
{
public:
	static Color GetColorForPlayer(int playerID);
	static int GetNextObjectIDForPlayer(int playerID);
	static void ToLowerCase(std::string &s);

	static int mIDCounter;
};