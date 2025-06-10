#pragma once

class Circle2D
{
public:
	Circle2D(const Vector2D & InCenter, float InRadius);
	const Vector2D & GetCenter() const;
	float GetRadius() const;
	~Circle2D();
private:
	Vector2D Center = {0, 0};
	float Radius = 0;
};
