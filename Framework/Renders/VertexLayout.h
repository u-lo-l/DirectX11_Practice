#pragma once

#include "framework.h"
#include "Math/Vector4.h"
#include "Math/Vector2D.h"

struct ShaderInputType
{
	virtual ~ShaderInputType() = 0;
};

//---------------------------------------------------------------------------//
struct Vertex : ShaderInputType
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	Vertex()
	: Position(0, 0, 0) {}
	~Vertex() override = default;
	Vector Position;
};
//---------------------------------------------------------------------------//
struct VertexColor : ShaderInputType
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	VertexColor()
	: Position(0, 0, 0)
	, Color(0,0,0,1) {}
	VertexColor(const Vector & Position, const Color & Color)
	: Position(Position.X, Position.Y, Position.Z)
	, Color(Color.R, Color.G, Color.B, Color.A) {}
	~VertexColor() override = default;
	Vector Position;
	Color Color;
};
//---------------------------------------------------------------------------//
struct VertexTexture : ShaderInputType
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	VertexTexture()
		: Position(0, 0, 0)
		, UV(0, 0) {}
	VertexTexture(const Vector& Position, const Vector2D& UV)
		: Position(Position)
		, UV(UV) {}
	~VertexTexture() override = default;
	Vector Position;
	Vector2D UV;
};

//---------------------------------------------------------------------------//
struct VertexTextureColor : ShaderInputType
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
	~VertexTextureColor() override = default;
	Vector Position;
	Vector2D UV;
	Color Color;
};
//---------------------------------------------------------------------------//
struct VertexNormal : ShaderInputType
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	VertexNormal()
		: Position(0, 0, 0), Normal(0, 0, 0) {}
	VertexNormal(const Vector& Position, const Vector& Normal)
		: Position(Position), Normal(Normal) {}
	~VertexNormal() override = default;
	Vector Position;
	Vector Normal;
};
//---------------------------------------------------------------------------//
struct VertexTextureNormal : ShaderInputType
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	VertexTextureNormal()
		: Position(0, 0, 0), UV(0, 0), Normal(0, 0, 0) {}
	VertexTextureNormal(const Vector& Position, const Vector2D & Uv, const Vector& Normal)
		: Position(Position), UV(Uv), Normal(Normal) {}
	~VertexTextureNormal() override = default;
	Vector Position;
	Vector2D UV;
	Vector Normal;
};

//---------------------------------------------------------------------------//

typedef struct VertexTextureColorNormalTangentBlend : ShaderInputType
{
	static void CreatInputLayout( vector<D3D11_INPUT_ELEMENT_DESC> & OutLayoutDescs );

	~VertexTextureColorNormalTangentBlend() override = default;
	Vector Position;
	Vector2D UV;
	Color Color;
	Vector Normal;
	Vector Tangent;
	Vector4 Indices {0, };
	Vector4 Weights {0, };
} ModelVertex;

//---------------------------------------------------------------------------//

