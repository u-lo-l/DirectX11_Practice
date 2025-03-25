// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
#pragma once
#include "Math/Vector.h"
#include "Math/Vector2D.h"
#include "Math/Vector4.h"

struct ShaderInputType { };

//---------------------------------------------------------------------------//
struct Vertex
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	Vertex()
	: Position(0, 0, 0) {}
	Vector Position;
};
//---------------------------------------------------------------------------//
struct VertexColor
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

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
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

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
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

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
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

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
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	VertexTextureNormal()
		: Position(0, 0, 0), UV(0, 0), Normal(0, 0, 0) {}
	VertexTextureNormal(const Vector& Position, const Vector2D & Uv, const Vector& Normal)
		: Position(Position), UV(Uv), Normal(Normal) {}
	Vector Position;
	Vector2D UV;
	Vector Normal;
};

//---------------------------------------------------------------------------//

typedef struct VertexTextureColorNormalTangentBlend
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	Vector Position;
	Vector2D UV;
	Color Color;
	Vector Normal;
	Vector Tangent;
	Vector4 Indices {0, };
	Vector4 Weights {0, };
} ModelVertex;

//---------------------------------------------------------------------------//

