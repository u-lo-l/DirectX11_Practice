#pragma once

struct ViewProjectionDesc;

class Billboard
{
public:
	static constexpr UINT MaxBillboardCount = 100000;
private:
	using VertexType = VertexBillboard;
	using ShaderType = HlslShader<VertexType>;
	using MaterialType = Material<VertexType>;
public:
	Billboard();
	virtual ~Billboard();

	void Tick();
	void Render() const;

	void Add(const Vector& InPosition, const Vector2D& InScale, UINT InMapIndex);
	void AddTexture(const wstring& InPath);

private:
	void UpdateTextures();
	void UpdateVBuffer();
	
	wstring ShaderName;
	ShaderType * BillboardShader = nullptr;
	
	Transform * WorldTF;

	vector<VertexType> Vertices;
	UINT VertexCount;

	VertexBuffer * VBuffer;

	vector<wstring> TextureNames;
	TextureArray * BillboardTextures;

	ViewProjectionDesc ViewProjectionData {};
	ConstantBuffer * GS_ViewProjectionBuffer;
};


