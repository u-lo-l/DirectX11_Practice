#pragma once
#include "Renders/Shader/HlslShader.hpp"

class SkySphere
{
	using VertexType = Vertex;
public:
	SkySphere(wstring InFilePath, float InRadius, UINT InSliceCount = 20);
	~SkySphere();

	void Tick();
	void Render();
	// ID3D11ShaderResourceView * GetSRV() const { return SkyTexture->GetSRV(); }
	const Texture * GetTexture() const { return SkyTexture; }

private:
	/*
	 * Create Sphere VertexBuffer
	 */
	void CreateVertexBuffer();
	void CreateIndexBuffer();

private:
	HlslShader<VertexType> * SkyShader;
	
	Transform * CameraTF;

	VertexBuffer * VBuffer;
	UINT VertexCount;

	IndexBuffer * IBuffer;
	UINT IndexCount;
private:
	float Radius;
	UINT SliceCount;
private:
	Texture * SkyTexture;
	// ID3D11ShaderResourceView * SkySRV;
};
