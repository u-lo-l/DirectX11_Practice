#include <Framework.h>
#include "Color.h"

Color::Color()
{
	A = R = G = B = 0.0f;
}

Color::Color( float Value )
{
	A = R = G = B = Value;
}

Color::Color( float r, float g, float b, float a )
{
	R = r;
	G = g;
	B = b;
	A = a;
}

Color::Color( const Vector4 & Value )
{
	R = Value.X;
	G = Value.Y;
	B = Value.Z;
	A = Value.W;
}

Color::Color( const Vector & Value, float Alpha )
{
	R = Value.X;
	G = Value.Y;
	B = Value.Z;
	A = Alpha;
}

Color::Color( UINT Rgba )
{
	A = ((Rgba >> 24) & 255) / 255.0f;
	B = ((Rgba >> 16) & 255) / 255.0f;
	G = ((Rgba >> 8) & 255) / 255.0f;
	R = (Rgba & 255) / 255.0f;
}

Color::Color( int rgba )
{
	A = static_cast<float>(rgba >> 24 & 255) / 255.0f;
	B = static_cast<float>(rgba >> 16 & 255) / 255.0f;
	G = static_cast<float>(rgba >> 8 & 255) / 255.0f;
	R = static_cast<float>(rgba & 255) / 255.0f;
}

Color::Color( const float * values )
{
	R = values[0];
	G = values[1];
	B = values[2];
	A = values[3];
}

Color::operator float *()
{
	return RGBA;
}

Color::operator const float *() const
{
	return RGBA;
}

bool Color::operator==( const Color & value2 ) const
{
	if (R == value2.R && G == value2.G && B == value2.B)
		return A == value2.A;
	else
		return false;
}

bool Color::operator!=( const Color & value2 ) const
{
	if (R == value2.R && G == value2.G && B == value2.B)
		return A != value2.A;
	else
		return true;
}

Color Color::operator+( const Color & value2 ) const
{
	Color color;
	color.R = R + value2.R;
	color.G = G + value2.G;
	color.B = B + value2.B;
	color.A = A + value2.A;

	return color;
}

Color Color::operator-( const Color & value2 ) const
{
	Color color;
	color.R = R - value2.R;
	color.G = G - value2.G;
	color.B = B - value2.B;
	color.A = A - value2.A;

	return color;
}

Color Color::operator*( const Color & value2 ) const
{
	Color color;
	color.R = R * value2.R;
	color.G = G * value2.G;
	color.B = B * value2.B;
	color.A = A * value2.A;

	return color;
}

Color Color::operator*( const float & scaleFactor ) const
{
	Color color;
	color.R = R * scaleFactor;
	color.G = G * scaleFactor;
	color.B = B * scaleFactor;
	color.A = A * scaleFactor;

	return color;
}

Color Color::operator/( const Color & value2 ) const
{
	Color color;
	color.R = R / value2.R;
	color.G = G / value2.G;
	color.B = B / value2.B;
	color.A = A / value2.A;

	return color;
}

Color Color::operator/( const float & divider ) const
{
	float num = 1.0f / divider;

	Color color;
	color.R = R * num;
	color.G = G * num;
	color.B = B * num;
	color.A = A * num;

	return color;
}

void Color::operator+=( const Color & Value2 )
{
	*this = *this + Value2;
}

void Color::operator-=( const Color & Value2 )
{
	*this = *this - Value2;
}

void Color::operator*=( const Color & Value2 )
{
	*this = *this * Value2;
}

void Color::operator*=( const float & ScaleFactor )
{
	*this = *this * ScaleFactor;
}

void Color::operator/=( const Color & Value2 )
{
	*this = *this / Value2;
}

void Color::operator/=( const float & Divider )
{
	*this = *this / Divider;
}

int Color::ToRgba() const
{
	const UINT a = static_cast<UINT>(A * 255.0f) & 255;
	const UINT r = static_cast<UINT>(R * 255.0f) & 255;
	const UINT g = static_cast<UINT>(G * 255.0f) & 255;
	const UINT b = static_cast<UINT>(B * 255.0f) & 255;

	UINT value = r;
	value |= g << 8;
	value |= b << 16;
	value |= a << 24;

	return static_cast<int>(value);
}


int Color::ToBgra() const
{
	const UINT a = static_cast<UINT>(A * 255.0f) & 255;
	const UINT r = static_cast<UINT>(R * 255.0f) & 255;
	const UINT g = static_cast<UINT>(G * 255.0f) & 255;
	const UINT b = static_cast<UINT>(B * 255.0f) & 255;

	UINT value = b;
	value |= g << 8;
	value |= r << 16;
	value |= a << 24;

	return static_cast<int>(value);
}

void Color::ToBgra( byte & r, byte & g, byte & b, byte & a ) const
{
	a = (byte)(A * 255.0f);
	r = (byte)(R * 255.0f);
	g = (byte)(G * 255.0f);
	b = (byte)(B * 255.0f);
}

Vector Color::ToVector3() const
{
	return Vector(R, G, B);
}

Vector4 Color::ToVector4() const
{
	return Vector4(R, G, B, A);
}

std::wstring Color::ToWString() const
{
	std::wstring temp = L"";
	temp += L" R:" + std::to_wstring(R);
	temp += L",G:" + std::to_wstring(G);
	temp += L",B:" + std::to_wstring(B);
	temp += L",A:" + std::to_wstring(A);

	return temp;
}

std::string Color::ToString() const
{
	std::string Temp = "";
	Temp += " R:" + std::to_string(R);
	Temp += ",G:" + std::to_string(G);
	Temp += ",B:" + std::to_string(B);
	Temp += ",A:" + std::to_string(A);

	return Temp;
}

Color Color::Add( const Color & Left, const Color & Right )
{
	return {Left.R + Right.R, Left.G + Right.G, Left.B + Right.B, Left.A + Right.A};
}

Color Color::Subtract( const Color & Left, const Color & Right )
{
	return {Left.R - Right.R, Left.G - Right.G, Left.B - Right.B, Left.A - Right.A};
}

Color Color::Modulate( const Color & Left, const Color & Right )
{
	return Color(Left.R * Right.R, Left.G * Right.G, Left.B * Right.B, Left.A * Right.A);
}

Color Color::Scale( const Color & value, float scale )
{
	return Color(value.R * scale, value.G * scale, value.B * scale, value.A * scale);
}

Color Color::Negate( const Color & value )
{
	return Color(1.0f - value.R, 1.0f - value.G, 1.0f - value.B, 1.0f - value.A);
}

Color Color::Clamp( const Color & val, const Color & min, const Color & max )
{
	float alpha = val.A;
	alpha = (alpha > max.A) ? max.A : alpha;
	alpha = (alpha < min.A) ? min.A : alpha;

	float red = val.R;
	red = (red > max.R) ? max.R : red;
	red = (red < min.R) ? min.R : red;

	float green = val.G;
	green = (green > max.G) ? max.G : green;
	green = (green < min.G) ? min.G : green;

	float blue = val.B;
	blue = (blue > max.B) ? max.B : blue;
	blue = (blue < min.B) ? min.B : blue;

	return {red, green, blue, alpha};
}

Color Color::Lerp( const Color & start, const Color & end, float amount )
{
	Color color;
	color.R = Math::Lerp(start.R, end.R, amount);
	color.G = Math::Lerp(start.G, end.G, amount);
	color.B = Math::Lerp(start.B, end.B, amount);
	color.A = Math::Lerp(start.A, end.A, amount);

	return color;
}

Color Color::SmoothStep( const Color & start, const Color & end, float amount )
{
	amount = Math::SmoothStep(amount);

	return Lerp(start, end, amount);
}

Color Color::Max( const Color & left, const Color & right )
{
	Color color;
	color.A = (left.A > right.A) ? left.A : right.A;
	color.R = (left.R > right.R) ? left.R : right.R;
	color.G = (left.G > right.G) ? left.G : right.G;
	color.B = (left.B > right.B) ? left.B : right.B;

	return color;
}

Color Color::Min( const Color & left, const Color & right )
{
	Color color;
	color.A = (left.A < right.A) ? left.A : right.A;
	color.R = (left.R < right.R) ? left.R : right.R;
	color.G = (left.G < right.G) ? left.G : right.G;
	color.B = (left.B < right.B) ? left.B : right.B;

	return color;
}

Color Color::AdjustContrast( const Color & value, float contrast )
{
	return Color(
		0.5f + contrast * (value.R - 0.5f),
		0.5f + contrast * (value.G - 0.5f),
		0.5f + contrast * (value.B - 0.5f),
		value.A
	);
}

Color Color::AdjustSaturation( const Color & value, float saturation )
{
	float grey = value.R * 0.2125f + value.G * 0.7154f + value.B * 0.0721f;

	return Color(
		grey + saturation * (value.R - grey),
		grey + saturation * (value.G - grey),
		grey + saturation * (value.B - grey),
		value.A
	);
}

Color Color::Premultiply( const Color & value )
{
	Color color;
	color.A = value.A;
	color.R = value.R * value.A;
	color.G = value.G * value.A;
	color.B = value.B * value.A;

	return color;
}


const Color Color::Aqua = Color(-256);
const Color Color::Beige = Color(-2296331);
const Color Color::Black = Color(-16777216);
const Color Color::Blue = Color(-65536);
const Color Color::Brown = Color(-14013787);
const Color Color::Chocolate = Color(-14784046);
const Color Color::CornflowerBlue = Color(-1206940);
const Color Color::Cyan = Color(-256);
const Color Color::DarkBlue = Color(-7667712);
const Color Color::DarkCyan = Color(-7632128);
const Color Color::DarkGray = Color(-5658199);
const Color Color::DarkGreen = Color(-16751616);
const Color Color::DarkOrange = Color(-16741121);
const Color Color::DarkRed = Color(-16777077);
const Color Color::DarkSeaGreen = Color(-7619441);
const Color Color::DeepPink = Color(-7138049);
const Color Color::DeepSkyBlue = Color(-16640);
const Color Color::GhostWhite = Color(-1800);
const Color Color::Gold = Color(-16721921);
const Color Color::Gray = Color(-8355712);
const Color Color::Green = Color(-16744448);
const Color Color::HotPink = Color(-4953601);
const Color Color::Indigo = Color(-8257461);
const Color Color::Ivory = Color(-983041);
const Color Color::Khaki = Color(-7543056);
const Color Color::Lavender = Color(-334106);
const Color Color::LightBlue = Color(-1648467);
const Color Color::LightCoral = Color(-8355600);
const Color Color::LightCyan = Color(-32);
const Color Color::LightGray = Color(-2894893);
const Color Color::LightGreen = Color(-7278960);
const Color Color::LightPink = Color(-4081921);
const Color Color::LightSeaGreen = Color(-5590496);
const Color Color::LightSkyBlue = Color(-340345);
const Color Color::LightYellow = Color(-2031617);
const Color Color::Lime = Color(-16711936);
const Color Color::Magenta = Color(-65281);
const Color Color::MediumBlue = Color(-3342336);
const Color Color::MediumPurple = Color(-2396013);
const Color Color::MediumSeaGreen = Color(-9325764);
const Color Color::MidnightBlue = Color(-9430759);
const Color Color::Navy = Color(-8388608);
const Color Color::Olive = Color(-16744320);
const Color Color::Orange = Color(-16734721);
const Color Color::OrangeRed = Color(-16759297);
const Color Color::Pink = Color(-3424001);
const Color Color::Purple = Color(-8388480);
const Color Color::Red = Color(-16776961);
const Color Color::RoyalBlue = Color(-2004671);
const Color Color::Salmon = Color(-9273094);
const Color Color::SeaGreen = Color(-11039954);
const Color Color::Silver = Color(-4144960);
const Color Color::SkyBlue = Color(-1323385);
const Color Color::SpringGreen = Color(-8388864);
const Color Color::Tomato = Color(-12098561);
const Color Color::Transparent = Color(0);
const Color Color::Violet = Color(-1146130);
const Color Color::Wheat = Color(-4989195);
const Color Color::White = Color(-1);
const Color Color::WhiteSmoke = Color(-657931);
const Color Color::Yellow = Color(-16711681);
const Color Color::YellowGreen = Color(-13447782);
