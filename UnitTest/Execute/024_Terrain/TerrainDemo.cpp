#include "Pch.h"
#include "TerrainDemo.h"

namespace Sdt
{
	void TerrainDemo::Initialize()
	{
		Camera * MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition(15, 10, -30);
		MainCamera->SetRotation(0, 0, 0);
		Landscape = new Terrain(L"17_TerrainNormal.fx", L"Terrain/Gray256.png");

		Drawer = new Shader(L"18_TerrainNormalVector.fx");
		WorldMatrix = Matrix::Identity;
		
		Terrain::TerrainVertexType * TerrainVertex = Landscape->GetVertice();
		const UINT Width = Landscape->GetWidth();
		const UINT Height = Landscape->GetHeight();
		
		const Color Color = Color::Red;
		for (UINT Z = 0; Z < Height; Z += 5)
		{
			for (UINT X = 0; X < Width; X += 5)
			{
				const UINT Index = X + Width * Z;
				Vector Start = TerrainVertex[Index].Position;
				Vector End = Start + TerrainVertex[Index].Normal * 2.f;
				
				// Vertice.emplace_back(Start, Color);
				// Vertice.emplace_back(End, Color);
				Vertice.push_back(VertexColor(Start, Color::Red));
				Vertice.push_back(VertexColor(End, Color::Red));
			}
		}
		VertexColor * Vertice2 = new VertexColor[Vertice.size()];
		copy(Vertice.begin(), Vertice.end(), Vertice2);
		VBuffer = new VertexBuffer(Vertice2, Vertice.size(), sizeof(VertexColor));
	}

	void TerrainDemo::Destroy()
	{
		SAFE_DELETE(Landscape);
	}

	void TerrainDemo::Tick()
	{
		Landscape->Tick();
		Drawer->AsMatrix("World")->SetMatrix(WorldMatrix);
		Drawer->AsMatrix("View")->SetMatrix(Context::Get()->GetViewMatrix());
		Drawer->AsMatrix("Projection")->SetMatrix(Context::Get()->GetProjectionMatrix());
	}

	void TerrainDemo::Render()
	{
		Landscape->SetPass(0);
		Landscape->Render();

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		VBuffer->BindToGPU();
		
		Drawer->Draw(0,0,Vertice.size());
	}
}
