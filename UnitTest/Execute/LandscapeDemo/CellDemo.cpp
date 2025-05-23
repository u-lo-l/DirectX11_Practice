#include "Pch.h"
#include "CellDemo.h"

namespace sdt
{
	void CellDemo::Initialize()
	{
		const Vector TerrainDimension = {1024, 512, 1024};
		constexpr UINT GridSize = 64;
		Sky = new SkySphere();
		Context::Get()->GetCamera()->SetPosition(-830, 1020, -441);
		Context::Get()->GetCamera()->SetRotation(17, 50, -0);

		LandScape::LandScapeDesc Desc =
		{
			TerrainDimension,
			128,
			GridSize,
			L"Terrain/GrandMountain/Height Map TIF.tif",
			{L"Terrain/Grass/Diffuse_1k.png", L"Terrain/Dirt/Diffuse_1k.jpg", L"Terrain/Rock/Diffuse_1k.png", L"Terrain/Sand/Diffuse_1k.png"},
			{L"Terrain/Grass/Normal_1k.png", L"Terrain/Dirt/Normal_1k.jpg", L"Terrain/Rock/Normal_1k.png", L"Terrain/Sand/Normal_1k.png"}
		};
		Terrain = new LandScape(Desc);

		Vector SeaDimension = Vector(1024, 10, 1024) * 2;
		float SeaLevel = 5.f;
		const Ocean::OceanDesc OceanDesc{
			SeaDimension,
			512,
			128,
			64,
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
		ImGui::Checkbox("Draw Terrain", &bDrawTerrain);
		ImGui::Checkbox("Draw Sea", &bDrawSea);
		Sky->Tick();
		if(!!Sea && bDrawSea) Sea->Tick();
		if(!!Terrain && bDrawTerrain) Terrain->Tick();
	}

	void CellDemo::Render()
	{
		Sky->Render();
		if(!!Sea && bDrawSea) Sea->Render(true);
		if(!!Terrain && bDrawTerrain) Terrain->Render(true);
	}

	void CellDemo::Destroy()
	{
		SAFE_DELETE(Terrain);
		SAFE_DELETE(Sky);
	}
}
