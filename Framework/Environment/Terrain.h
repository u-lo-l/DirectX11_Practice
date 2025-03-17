#pragma once
#include "Interface/IPlaceable.h"
#include "Interface/IRenderable.h"
#include "Renders/HlslShader.hpp"

class Terrain
{
public:
	using TerrainVertexType = VertexNormal;
	Terrain(const wstring & InShaderFileName, const wstring& InHeightMapFileName);
	~Terrain();

	void Tick();
	void Render() const;

	void SetPass( const int InPass ) { Pass = InPass; }
	UINT GetWidth() const { return Width; }
	UINT GetHeight() const { return Height; }
	void GetPositionY(Vector& InPosition) const;
	
private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();
	void CreateBuffer();
private:
	int Pass = 0;
	// Shader * Drawer = nullptr;
	HlslShader<TerrainVertexType> * Shader = nullptr;
	Texture* HeightMap;

	UINT Width, Height;
	UINT VertexCount;
	vector<TerrainVertexType> Vertices;
	
	UINT IndexCount;
	vector<UINT> Indices;

	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

	Matrix WorldMat;
	ID3D11Buffer * WVPCBuffer = nullptr;
	
private :
	static bool IntersectRayTriangle(
		const DirectX::XMFLOAT3& rayPos,  // 광선의 시작점
		const DirectX::XMFLOAT3& rayDir,  // 광선의 방향
		const DirectX::XMFLOAT3& v0,      // 삼각형의 첫 번째 정점
		const DirectX::XMFLOAT3& v1,      // 삼각형의 두 번째 정점
		const DirectX::XMFLOAT3& v2,      // 삼각형의 세 번째 정점
		float* u,                // 교차 지점의 Barycentric 좌표 u
		float* v,                // 교차 지점의 Barycentric 좌표 v
		float* dist                 // 교차 지점까지의 거리
	);
};
