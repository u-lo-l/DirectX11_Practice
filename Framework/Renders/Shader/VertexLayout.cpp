#include "framework.h"
#include "VertexLayout.h"

#define INPUT_ELEMENT_VERTEX_FLOAT(name)	{name, 0, DXGI_FORMAT_R32_FLOAT, RenderingShader::VertexSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
#define INPUT_ELEMENT_VERTEX_FLOAT2(name)	{name, 0, DXGI_FORMAT_R32G32_FLOAT, RenderingShader::VertexSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
#define INPUT_ELEMENT_VERTEX_FLOAT3(name)	{name, 0, DXGI_FORMAT_R32G32B32_FLOAT, RenderingShader::VertexSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
#define INPUT_ELEMENT_VERTEX_FLOAT4(name)	{name, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, RenderingShader::VertexSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
#define INPUT_ELEMENT_VERTEX_UINT(name)		{name, 0, DXGI_FORMAT_R8_UINT, RenderingShader::VertexSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}

#define INPUT_ELEMENT_INSTANCE_MATRIX	{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, RenderingShader::InstanceSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},\
										{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, RenderingShader::InstanceSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},\
										{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, RenderingShader::InstanceSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},\
										{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, RenderingShader::InstanceSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}

const vector<D3D11_INPUT_ELEMENT_DESC> & Vertex::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION")
	};
	return InputLayoutElements;
}

// float3 Position, float4 Color
const vector<D3D11_INPUT_ELEMENT_DESC> & VertexColor::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT4("COLOR"),
		INPUT_ELEMENT_INSTANCE_MATRIX
	};
	return InputLayoutElements;
}

// float3 Position, float2 UV
const vector<D3D11_INPUT_ELEMENT_DESC> & VertexTexture::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT2("UV")
	};
	return InputLayoutElements;
}

// float3 position, float2 UV, float4 Color
const vector<D3D11_INPUT_ELEMENT_DESC> & VertexTextureColor::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT2("UV"),
		INPUT_ELEMENT_VERTEX_FLOAT4("COLOR")
	};
	return InputLayoutElements;
}

// float3 Position, float3 Normal
const vector<D3D11_INPUT_ELEMENT_DESC> & VertexNormal::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT3("NORMAL"),
		INPUT_ELEMENT_INSTANCE_MATRIX
	};
	return InputLayoutElements;
}

// float3 Position, float2 UV, float3 Normal
const vector<D3D11_INPUT_ELEMENT_DESC> &  VertexTextureNormal::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT2("UV"),
		INPUT_ELEMENT_VERTEX_FLOAT3("NORMAL"),
		INPUT_ELEMENT_INSTANCE_MATRIX
	};
	return InputLayoutElements;
}

// float3 Position, float2 UV, float4 Color, float3 Normal, float3 Tangent, float4 Indices, float4 Weights;
const vector<D3D11_INPUT_ELEMENT_DESC> & SkeletalMeshVertex::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT2("UV"),
		INPUT_ELEMENT_VERTEX_FLOAT4("COLOR"),
		INPUT_ELEMENT_VERTEX_FLOAT3("NORMAL"),
		INPUT_ELEMENT_VERTEX_FLOAT3("TANGENT"),
		INPUT_ELEMENT_VERTEX_FLOAT4("BLENDINDICES"),
		INPUT_ELEMENT_VERTEX_FLOAT4("BLENDWEIGHTS"),
		INPUT_ELEMENT_INSTANCE_MATRIX
	};
	return InputLayoutElements;
}

const vector<D3D11_INPUT_ELEMENT_DESC> &  VertexBillboard::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT2("SCALE"),
		INPUT_ELEMENT_VERTEX_UINT("MAPINDEX")
	};
	return InputLayoutElements;
}

const vector<D3D11_INPUT_ELEMENT_DESC> &  VertexPrecipitation::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT2("SCALE"),
		INPUT_ELEMENT_VERTEX_FLOAT2("RANDOM"),
	};
	return InputLayoutElements;
}

const vector<D3D11_INPUT_ELEMENT_DESC> &  VertexParticle::GetVertexInputLayoutElements()
{
	static vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutElements
	{
		INPUT_ELEMENT_VERTEX_FLOAT3("POSITION"),
		INPUT_ELEMENT_VERTEX_FLOAT2("START_VELOCITY"),
		INPUT_ELEMENT_VERTEX_FLOAT2("END_VELOCITY"),
		INPUT_ELEMENT_VERTEX_FLOAT3("RANDOM"),
		INPUT_ELEMENT_VERTEX_FLOAT("TIME"),
	};
	return InputLayoutElements;
}
