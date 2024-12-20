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
	VertexColor(Vector P, Color C)
	: Position(P)
	, Color(C) {}
	Vector Position;
	Color Color;
};