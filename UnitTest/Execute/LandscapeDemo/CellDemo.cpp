#include "Pch.h"
#include "CellDemo.h"

namespace sdt
{
	void CellDemo::Initialize()
	{
		const Vector TerrainDimension = {2560, 1024, 2560};
		const UINT GridSize = static_cast<UINT>(powf(2, 6));
		Sky = new SkySphere();
		Context::Get()->GetCamera()->SetPosition(-830, 1020, -441);
		Context::Get()->GetCamera()->SetRotation(17, 50, -0);

		LandScape::LandScapeDesc Desc =
		{
			TerrainDimension,
			256,
			GridSize,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		// Terrain = new LandScape(Desc);

		Vector SeaDimension = Vector(1024, 10, 1024) * 16;
		float SeaLevel = 5.f;
		const Ocean::OceanDesc OceanDesc{
			SeaDimension,
			256,
			1024 * 16,
			16,
			SeaLevel,
			Sky->GetTexture(),
			nullptr,
			{0, 0},
			{0, 0},
			0,
			{-15, -20}
		};
		Sea = new Ocean(OceanDesc);

		if (!!Sea && !!Terrain)
		{
			const Vector2D TerrainCenter = {Terrain->GetDimension().X * 0.5f, Terrain->GetDimension().Z * 0.5f};
			Sea->SetWorldPosition({TerrainCenter.X - SeaDimension.X * 0.5f, SeaLevel, TerrainCenter.Y - SeaDimension.Z * 0.5f});
		}
	}

	void CellDemo::Tick()
	{
		Sky->Tick();
		if(!!Sea) Sea->Tick();
		if(!!Terrain) Terrain->Tick();
	}

	void CellDemo::Render()
	{
		Sky->Render();
		if(!!Sea) Sea->Render(true);
		if(!!Terrain) Terrain->Render(true);
	}

	void CellDemo::Destroy()
	{
		SAFE_DELETE(Terrain);
		SAFE_DELETE(Sky);
	}
}
