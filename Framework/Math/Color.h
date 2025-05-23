#pragma once

class Vector;
class Vector4;

class Color
{
public:
	Color();
	explicit Color(float Value);
	Color(float r, float g, float b, float a);
	explicit Color(const Vector4& Value);
	Color(const Color& Value);
	Color& operator=(const Color& Value);
	Color(const Vector& Value, float Alpha);
	explicit Color(UINT Rgba);
	explicit Color(int rgba);
	explicit Color(const float* values);

	// ReSharper disable once CppNonExplicitConversionOperator
	operator float* ();
	// ReSharper disable once CppNonExplicitConversionOperator
	operator const float* () const;

	bool operator ==(const Color& value2) const;
	bool operator !=(const Color& value2) const;

	Color operator +(const Color& value2) const;
	Color operator -(const Color& value2) const;
	Color operator *(const Color& value2) const;
	Color operator *(const float& scaleFactor) const;
	Color operator /(const Color& value2) const;
	Color operator /(const float& divider) const;

	void operator +=(const Color& Value2);
	void operator -=(const Color& Value2);
	void operator *=(const Color& Value2);
	void operator *=(const float& ScaleFactor);
	void operator /=(const Color& Value2);
	void operator /=(const float& Divider);

	int ToRgba() const;
	int ToBgra() const;
	void ToBgra(byte& r, byte& g, byte& b, byte& a) const;

	Vector ToVector3() const;
	Vector4 ToVector4() const;

	std::wstring ToWString() const;
	std::string ToString() const;

	static Color Add(const Color& Left, const Color& Right);
	static Color Subtract(const Color& Left, const Color& Right);
	static Color Modulate(const Color& Left, const Color& Right);
	static Color Scale( const Color & value, float scale);
	static Color Negate( const Color & value);

	static Color Clamp( const Color & val, const Color & min, const Color & max);
	static Color Lerp( const Color & start, const Color & end, float amount);
	static Color SmoothStep( const Color & start, const Color & end, float amount);
	static Color Max( const Color & left, const Color & right);
	static Color Min( const Color & left, const Color & right);

	static Color AdjustContrast( const Color & value, float contrast);
	static Color AdjustSaturation( const Color & value, float saturation);
	static Color Premultiply( const Color & value);

public:
	static const Color Aqua;
	static const Color Beige;
	static const Color Black;
	static const Color Blue;
	static const Color Brown;
	static const Color Chocolate;
	static const Color CornflowerBlue;
	static const Color Cyan;
	static const Color DarkBlue;
	static const Color DarkCyan;
	static const Color DarkGray;
	static const Color DarkGreen;
	static const Color DarkOrange;
	static const Color DarkRed;
	static const Color DarkSeaGreen;
	static const Color DeepPink;
	static const Color DeepSkyBlue;
	static const Color GhostWhite;
	static const Color Gold;
	static const Color Gray;
	static const Color Green;
	static const Color HotPink;
	static const Color Indigo;
	static const Color Ivory;
	static const Color Khaki;
	static const Color Lavender;
	static const Color LightBlue;
	static const Color LightCoral;
	static const Color LightCyan;
	static const Color LightGray;
	static const Color LightGreen;
	static const Color LightPink;
	static const Color LightSeaGreen;
	static const Color LightSkyBlue;
	static const Color LightYellow;
	static const Color Lime;
	static const Color Magenta;
	static const Color MediumBlue;
	static const Color MediumPurple;
	static const Color MediumSeaGreen;
	static const Color MidnightBlue;
	static const Color Navy;
	static const Color Olive;
	static const Color Orange;
	static const Color OrangeRed;
	static const Color Pink;
	static const Color Purple;
	static const Color Red;
	static const Color RoyalBlue;
	static const Color Salmon;
	static const Color SeaGreen;
	static const Color Silver;
	static const Color SkyBlue;
	static const Color SpringGreen;
	static const Color Tomato;
	static const Color Transparent;
	static const Color Violet;
	static const Color Wheat;
	static const Color White;
	static const Color WhiteSmoke;
	static const Color Yellow;
	static const Color YellowGreen;

public:
	union
	{
		struct
		{
			float R, G, B, A;
		};

		float RGBA[4];
	};
};