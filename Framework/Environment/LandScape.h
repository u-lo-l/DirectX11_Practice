#pragma once
#include "Renders/Shader/HlslShader.h"

class ConstantBuffer;
class Transform;
class IndexBuffer;
class Texture;
class VertexBuffer;
struct VertexTextureNormal;

class LandScape
{
public:
	struct LandScapeDesc
	{
		wstring HeightMapName;
		wstring DiffuseMapName;
		wstring BumpMapName;
		wstring NormalMapName;
	};
private:
	friend class TerrainQuadTree;
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
		float HeightScaler = 100.f;
		
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
	explicit LandScape(const LandScapeDesc & Desc, UINT PatchSize = 16);
	explicit LandScape(const wstring & InHeightMapFilename, UINT PatchSize = 16);
	~LandScape();

	void Tick();
	void Render() const;

	UINT GetWidth() const;
	UINT GetHeight() const;
	float GetAltitude(const Vector2D & InPositionXZ) const;
	UINT GetPatchSize() const;
	const Texture * GetHeightMap() const;
	float GetHeightScale() const;
	void SetHeightScale(float InHeightScale);
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

	Texture * HeightMap = nullptr;
	Texture * DiffuseMap = nullptr;
	Texture * BumpMap = nullptr;
	Texture * NormalMap = nullptr;
	
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

	Transform * Tf;
	ConstantBuffer * WVPCBuffer = nullptr;
	ConstantBuffer * HeightScalerCBuffer= nullptr;
	ConstantBuffer * LightDirectionCBuffer= nullptr;
};


