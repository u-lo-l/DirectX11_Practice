#pragma once

class LandScape_QuadTree
{
public:
	// for Initialize
	struct LandScapeDesc;
private:
	using VertexType = VertexTextureNormal;
	using CellVertexType = VertexColor; 
	class LandScapeCell;
	// for Render
	struct LandScapeBlendingDesc;
	struct LandScapeTessellationDesc;
public:
	explicit LandScape_QuadTree(const LandScapeDesc & InDesc);
	~LandScape_QuadTree();
	void Tick();
	void Render(bool bDrawBoundary = false);
private:
	void SetupShaders();
	void SetupResources();
	void SetupCells();
	vector<LandScapeCell *> Cells;
	vector<Vector> CellPositions;
	InstanceBuffer * IBuffer;
	
	HlslShader<VertexType> * CellRenderer = nullptr;
	HlslShader<CellVertexType> * BoundaryRenderer = nullptr;
	
	WVPDesc * MatrixData = nullptr;	// VDHP
	DirectionalLightDesc * LightData = nullptr; // P
	LandScapeBlendingDesc * BlendingData = nullptr; // DH
	LandScapeTessellationDesc * TessellationData = nullptr; // DH

	ConstantBuffer * CB_WVP = nullptr; // 0
	ConstantBuffer * CB_Light = nullptr; // 1
	ConstantBuffer * CB_Blending = nullptr; // 2
	ConstantBuffer * CB_Tessellation = nullptr; // 3
};

class LandScape_QuadTree::LandScapeCell
{
private:
	using VertexType = VertexTextureNormal;
public:
	LandScapeCell(Vector CellExtend, Vector LocalPosition, float GridSize);
	~LandScapeCell();
	void Tick();
	void Render(HlslShader<VertexType> * InShader);
private:
	Box * BoundingBox; // Transform 포함
	
	LandScapeCell * ParentCell = nullptr;
	LandScapeCell * ChildCell[4] = {nullptr, };
	
	vector<VertexType> Vertices;
	vector<UINT> Indices;
	VertexBuffer * VertexBuffer = nullptr;
	IndexBuffer  * IndexBuffer = nullptr;
};

struct LandScape_QuadTree::LandScapeDesc
{
	Vector	Extend; // x, z : Horizontal, y : Vertical
	UINT	CellSize;  // Should Be Power Of 2
	float	GridSize;
	wstring HeightMap;
	vector<wstring> DiffuseMaps;
	vector<wstring> NormalMaps;
};

struct LandScape_QuadTree::LandScapeBlendingDesc
{
	float NearSize = 2.0f;
	float FarSize = 0.1f;
	float StartOffset = -1000.f;
	float Range = 5000.f;
	// Perlin Noise
	float NoiseAmount = 1.f;
	float NoisePower  = 1.f;
	// Slope Based
	float SlopBias = 45;
	float SlopSharpness = 3;
	// Altitude Base
	float LowHeight = 10;
	float HighHeight = 100;
	float HeightSharpness = 1;
	// Padding
	float Padding;
};

struct LandScape_QuadTree::LandScapeTessellationDesc
{
	Vector CameraWorldPosition;
	float HeightScaler = 100.f;
		
	float TerrainSize;
	float GridSize;
	float TextureSize;
	float Padding;
		
	Vector2D LODRange;
	Vector2D TexelSize;
		
	float  ScreenDistance;
	float  ScreenDiagonal;
	UINT   DiffuseMapCount = 0;
	UINT   NormalMapCount  = 0;
};