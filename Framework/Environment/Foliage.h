#pragma once

class Foliage : public ARenderable
{
private:
	using VertexType = VertexFoliage;
public:
	struct Desc
	{
		string Name;
		string MaterialName;
		string ShaderName;
		const LandScape * TargetTerrain;
		Vector2D AltitudeRange = {0.f, 100.f};
		float Stride = 1.f;
		float Scaler = 1.f;
		wstring DensityMapName;
		vector<wstring> Textures;
	};
	explicit Foliage(const Desc & InDesc);
	virtual ~Foliage() override;
	virtual void BindResources() const override;
	void Tick();
private:
	void CreateVertices(float TerrainDimensionX, float TerrainDimensionZ, UINT CellSize);
	struct CB_PerFoliageDesc
	{
		Matrix TerrainBaseWorldTF;

		float  HeightScaler;
		Vector DistanceRange;
		
		Vector2D AltitudeRange;
		Vector2D TerrainMapSize;
	} CB_PerFoliageData;
	
	Desc Info;
	UINT CellSize;
	vector<VertexType> Vertices = {};
	
	const RWTexture2D * NormalMap = nullptr;
};
