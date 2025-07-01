#pragma once

class TerrainCell;

class LandScape
{
public:
	// for Initialize
	struct LandScapeDesc
	{
		Vector	Dimension;				// Dimension Of Whole Terrain
		UINT	CellSize;				// Should Be Power Of 2
		float	GridSize;
		wstring HeightMapName;			// Relative Path to W_TEXTURE_PATH
		vector<wstring> DiffuseMaps;	// Relative Path to W_TEXTURE_PATH
		vector<wstring> NormalMaps;		// Relative Path to W_TEXTURE_PATH
	};
	explicit LandScape(const LandScapeDesc & InDesc);
	~LandScape();
	void Tick();
	const Vector & GetDimension() const;
	const Texture * GetHeightMap() const;
	const RWTexture2D * GetNormalMap() const;
	const RWTexture2D * GetTangentMap() const;
	UINT GetCellSize() const;
	Transform * GetTransform() const;
	float GetHeightScaler() const;

private:
	void SetupShaders();
	void SetupResources(const LandScapeDesc& InDesc);
	void SetupCells(const LandScapeDesc& InDesc);
	LandScapeDesc Info;

	TerrainCell * CellInstance = nullptr;
	// BondingVolume
	// Foliage
	ComputeShader * NormalMapGenerator = nullptr;
	Transform * Tf;
	Texture * HeightMap;
	TerrainMaterial * Mat;
};

