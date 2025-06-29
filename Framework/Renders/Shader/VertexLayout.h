// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions

#pragma once
#include "Math/Vector.h"
#include "Math/Vector2D.h"
#include "Math/Vector4.h"

//---------------------------------------------------------------------------//

struct Vertex
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

	Vertex()
	: Position(0, 0, 0) {}
	explicit Vertex(const Vector & Position)
	: Position(Position) {}
	Vector Position;
};

//---------------------------------------------------------------------------//

struct VertexColor
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

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
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

	VertexTexture()
		: Position(0, 0, 0)
		, UV(0, 0) {}
	VertexTexture(const Vector& Position, const Vector2D& UV)
		: Position(Position)
		, UV(UV) {}
	Vector Position;
	Vector2D UV;
};

//---------------------------------------------------------------------------//

struct VertexTextureColor
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

	VertexTextureColor()
		: Position(0, 0, 0)
		, UV(0, 0)
		, Color(0, 0, 0, 1)
	{}
	VertexTextureColor(const Vector& Position, const Vector2D& UV, const Color& Color)
		: Position(Position)
		, UV(UV)
		, Color(Color)
	{}
	Vector Position;
	Vector2D UV;
	Color Color;
};

//---------------------------------------------------------------------------//

struct VertexNormal
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

	VertexNormal()
		: Position(0, 0, 0), Normal(0, 0, 0) {}
	VertexNormal(const Vector& Position, const Vector& Normal)
		: Position(Position), Normal(Normal) {}
	Vector Position;
	Vector Normal;
};

//---------------------------------------------------------------------------//

struct VertexTextureNormal
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

	VertexTextureNormal()
		: Position(0, 0, 0), UV(0, 0), Normal(0, 0, 0) {}
	VertexTextureNormal(const Vector& Position, const Vector2D & Uv, const Vector& Normal)
		: Position(Position), UV(Uv), Normal(Normal) {}
	Vector Position;
	Vector2D UV;
	Vector Normal;
};

struct VertexTerrainCell
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

	VertexTerrainCell()
		: Position(0, 0, 0){}
	VertexTerrainCell(const Vector& Position, const Vector2D & UV)
		: Position(Position), UV(UV) {}
	Vector Position;
	Vector2D UV;
};

//---------------------------------------------------------------------------//

struct VertexStaticMesh
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();
	Vector Position;
	Vector2D UV;
	Color Color;
	Vector Normal;
	Vector Tangent;
};

//---------------------------------------------------------------------------//

struct VertexSkeletalMesh
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();
	Vector Position;
	Vector2D UV;
	Color Color;
	Vector Normal;
	Vector Tangent;
	Vector4 Indices {0, };
	Vector4 Weights {0, };
};

//---------------------------------------------------------------------------//

struct VertexBillboard
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

	VertexBillboard()
		: Position(0, 0, 0), Scale(0, 0), MapIndex(0){}
	VertexBillboard(const Vector& Position, const Vector2D& Scale, const UINT MapIndex)
		: Position(Position), Scale(Scale), MapIndex(MapIndex){}
	
	Vector Position;
	Vector2D Scale;
	UINT MapIndex;
};

struct VertexFoliage
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();

	VertexFoliage()
		: Position(0, 0, 0), Scale(0, 0), MapIndex(0), Random(0) {}
	VertexFoliage(const Vector& Position, const Vector2D& Scale, const UINT MapIndex, const float InRandom)
		: Position(Position), Scale(Scale), MapIndex(MapIndex), Random(InRandom) {}
	
	Vector Position;
	Vector2D Scale;
	UINT MapIndex;
	float Random;
};

//---------------------------------------------------------------------------//

// Vertex For Rain, Snow, Hail
struct VertexPrecipitation
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();
	VertexPrecipitation()
		: Position(0, 0, 0), Scale(0, 0), Random(0,0) {}
	VertexPrecipitation(const Vector& Position, const Vector2D& Scale, const Vector2D& Random)
		: Position(Position), Scale(Scale), Random(Random) {}
	
	Vector Position;
	Vector2D Scale;
	Vector2D Random;
};

//---------------------------------------------------------------------------//
struct VertexParticle
{
	static const vector<D3D11_INPUT_ELEMENT_DESC> & GetVertexInputLayoutElements();
	VertexParticle()
		: Position(0, 0, 0), StartVelocity(0, 0, 0), EndVelocity(0, 0, 0), Random(0, 0, 0, 0), SpawnedTime(0.f) {}
	VertexParticle(const Vector& InPosition, const Vector& InStartVelocity, const Vector& InEndVelocity,const Vector4& InRandom, const float InTime)
		: Position(InPosition), StartVelocity(InStartVelocity),  EndVelocity(InEndVelocity),Random(InRandom), SpawnedTime(InTime) {}
	
	Vector Position;
	Vector StartVelocity;
	Vector EndVelocity;
	Vector4 Random;		// each elements value between [0, 1]
	float SpawnedTime;
};