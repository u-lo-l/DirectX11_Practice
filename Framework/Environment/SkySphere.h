#pragma once
#include "Renders/Shader/HlslShader.hpp"

class SkySphere
{
	using VertexType = Vertex;
	using ShaderType = HlslShader<VertexType>;
public:
	SkySphere(wstring InFilePath, float InRadius, UINT InSliceCount = 20);
	~SkySphere();

	void Tick();
	void Render();
private:
	/*
	 * Create Sphere VertexBuffer
	 */
	void CreateVertexBuffer();
	void CreateIndexBuffer();

private:
	ShaderType * SkyShader;
	
	Transform * World;

	VertexBuffer * VBuffer;
	UINT VertexCount;

	IndexBuffer * IBuffer;
	UINT IndexCount;
private:
	float Radius;
	UINT SliceCount;
private:
	ID3D11ShaderResourceView * SkyBoxSRV;
};
