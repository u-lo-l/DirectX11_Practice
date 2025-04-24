#include "Pch.h"
#include "TerrainTessellationDemo.h"
namespace sdt
{
	void TerrainTessellationDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 419, 13, 455 );
		MainCamera->SetRotation( 177, -33, -180);
		
		Terrain = new Terrain2(L"Terrain/Gray512.png", 64);
	}

	void TerrainTessellationDemo::Destroy()
	{
		SAFE_DELETE(Terrain);
	}

	void TerrainTessellationDemo::Tick()
	{
		Terrain->Tick();
	}

	void TerrainTessellationDemo::Render()
	{
		Terrain->Render();
	}
}
