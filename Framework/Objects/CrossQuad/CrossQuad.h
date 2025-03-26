#pragma once

class CrossQuad
{
public:
	CrossQuad();
	~CrossQuad();
private:
	using VertexType = VertexBillboard;
	using ShaderType = HlslShader<VertexType>;
	using MaterialType = Material<VertexType>;
	
public:
	void Tick();
	void Render() const;

	void Add(const Vector& InPosition, const Vector2D& InScale, UINT InMapIndex);
	void AddTexture(const wstring& InPath);

private:
	void UpdateTextures();
	void UpdateVBuffer();
	
	wstring ShaderName;
	ShaderType * CrossQuadShader = nullptr;
	
	Transform * WorldTF;

	vector<VertexType> Vertices;
	UINT VertexCount;

	VertexBuffer * VBuffer;

	vector<wstring> TextureNames;
	TextureArray * CrossQuadTextures;

	ViewProjectionDesc ViewProjectionData {};
	ConstantBuffer * GS_ViewProjectionBuffer;
};
