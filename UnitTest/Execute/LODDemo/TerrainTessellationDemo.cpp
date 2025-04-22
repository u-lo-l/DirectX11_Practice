#include "Pch.h"
#include "TerrainTessellationDemo.h"
namespace sdt
{
	void TerrainTessellationDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 150, 50, -150 );
		MainCamera->SetRotation( 0, 0, 0);
		
		Terrain = new Terrain2(L"DisplacementTest.png", 32);
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
