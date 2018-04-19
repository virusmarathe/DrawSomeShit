#pragma once
class Vector2
{
public:
	Vector2();
	Vector2(float x, float y);
	~Vector2();

	Vector2 operator+(Vector2& v2) { return Vector2(X + v2.X, Y + v2.Y); }
	Vector2 operator-(Vector2& v2) { return Vector2(X - v2.X, Y - v2.Y); }

	float distSqr(Vector2 v2) { return (v2.X - X) * (v2.X - X) + (v2.Y - Y)*(v2.Y - Y); }

	float X;
	float Y;
};
