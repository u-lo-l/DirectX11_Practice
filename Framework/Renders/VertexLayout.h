#pragma once

#include "Framework.h"
#include "Core/Vector4.h"
#include "Core/Vector2D.h"

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
		, UV(0, 0) {
	}
	VertexTexture(const Vector& Position, const Vector2D& UV)
		: Position(Position)
		, UV(UV) {
	}
	Vector Position;
	Vector2D UV;
};
//---------------------------------------------------------------------------//
struct VertexTextureColor
{
	VertexTextureColor()
		: Position(0, 0, 0)
		, UV(0, 0)
		, Color(0, 0, 0, 1)
	{
	}
	VertexTextureColor(const Vector& Position, const Vector2D& UV, const Color& Color)
		: Position(Position)
		, UV(UV)
		, Color(Color)
	{
	}
	Vector Position;
	Vector2D UV;
	Color Color;
};
//---------------------------------------------------------------------------//
struct VertexNormal
{
	VertexNormal()
		: Position(0, 0, 0), Normal(0, 0, 0) {}
	VertexNormal(const Vector& Position, const Vector& Normal)
		: Position(Position), Normal(Normal) {}
	Vector Position;
	Vector Normal;
};
//---------------------------------------------------------------------------//
typedef struct VertexTextureColorNormalTangentBlend
{
	Vector Position;
	Vector2D UV;
	Color Color;
	Vector Normal;
	Vector Tangent;
	Vector4 Indices;
	Vector4 Weights;
} ModelVertex;