#pragma once

#include "Framework.h"

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
	: Position(Position.x, Position.y, Position.z)
	, Color(Color.r, Color.g, Color.b, Color.a) {}
	Vector Position;
	Color Color;
};