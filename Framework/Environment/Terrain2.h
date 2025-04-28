#pragma once
#include "Renders/Shader/HlslShader.h"

class ConstantBuffer;
class Transform;
class IndexBuffer;
class Texture;
class VertexBuffer;
struct VertexTextureNormal;

class Terrain2
{
	using VertexType = VertexTextureNormal;
	struct WVPDesc
	{
		Matrix World;
		Matrix View;
		Matrix Projection;
	};
	struct TerrainTessDesc
	{
		Vector CameraPosition;
		float HeightScaler = 60.f;
		
		Vector2D LODRange;
		Vector2D TexelSize;
		
		float ScreenDistance;
		float ScreenDiagonal;
		float Padding[2];
	};
	struct LightDirectionDesc
	{
		Vector Direction;
		float Padding;
	};
	Vector2D LODRange;
public:
	explicit Terrain2(const wstring & InHeightMapFilename, UINT PatchSize = 16);
	~Terrain2();

	void Tick();
	void Render() const;

	UINT GetWidth() const;
	UINT GetHeight() const;
	float GetAltitude(const Vector2D & InPositionXZ) const;
	UINT GetPatchSize() const;
	const Texture * GetHeightMap() const;
	float GetHeightScale() const;
	// void SetPatchSize(UINT InPatchSize);
private:
	void CreateVertex();
	void CreateIndex();

	UINT PatchSize = 4;
	vector<VertexType> Vertices;
	vector<UINT> Indices;
	WVPDesc WVP;
	TerrainTessDesc TerrainTessData;
	LightDirectionDesc LightDirectionData;
	
	HlslShader<VertexType> * Shader;
	Texture * HeightMap;
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

	Transform * Tf;
	ConstantBuffer * WVPCBuffer = nullptr;
	ConstantBuffer * HeightScalerCBuffer= nullptr;
	ConstantBuffer * LightDirectionCBuffer= nullptr;
};


