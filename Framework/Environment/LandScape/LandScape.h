#pragma once

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
	// void Render(bool bDrawBoundary = false) const;
	const Vector & GetDimension() const { return Data.Dimension; }
	// const Texture * GetHeightMap() const { return CellInstance->GetHeightMap(); }
private:
	void SetupShaders();
	void SetupResources(const LandScapeDesc& InDesc);
	void SetupCells(const LandScapeDesc& InDesc);
	LandScapeDesc Data;

	TerrainCellTest * CellInstance = nullptr;
	// BondingVolume
	// Foliage
	ComputeShader   * NormalMapGenerator = nullptr;
	Transform       * Tf;
	Texture * HeightMap;
};

