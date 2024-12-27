#pragma once

#include "Framework.h"

#include "../Core/Vector2D.h"

//---------------------------------------------------------------------------//
struct Vertex
{
	Vertex()
	: Position(0, 0, 0) {}
	Vector Position;
};
//---------------------------------------------------------------------------//
struct VertexColor
{
	VertexColor()
	: Position(0, 0, 0)
	, Color(0,0,0,1) {}
	VertexColor(const Vector & Position, const Color & Color)
	: Position(Position.X, Position.Y, Position.Z)
	, Color(Color.R, Color.G, Color.B, Color.A) {}
	Vector Position;
	Color Color;
};
//---------------------------------------------------------------------------//
struct VertexTexture
{
	VertexTexture()
		: Position(0, 0, 0)
		, UV(0, 0) {}
	VertexTexture(const Vector& Position, const Vector2D& UV)
		: Position(Position)
		, UV(UV) {}
	Vector Position;
	Vector2D UV;

};
