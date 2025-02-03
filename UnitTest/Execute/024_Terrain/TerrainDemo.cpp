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
		Drawer2 = new Shader(L"18_TerrainNormalVector.fx");
		WorldMatrix = Matrix::Identity;
		
		Terrain::TerrainVertexType * TerrainVertex = Landscape->GetVertice();
		const UINT Width = Landscape->GetWidth();
		const UINT Height = Landscape->GetHeight();
		
		for (UINT Z = 0; Z < Height; Z += 5)
		{
			for (UINT X = 0; X < Width; X += 5)
			{
				const UINT Index = X + Width * Z;
				Vector Start = TerrainVertex[Index].Position;
				Vector End = Start + TerrainVertex[Index].Normal * 2.f;
				
				Vertice.emplace_back(Start, Color::Red);
				Vertice.emplace_back(End, Color::Red);
			}
		}
		VertexColor * TempVertice = new VertexColor[Vertice.size()];
		copy(Vertice.begin(), Vertice.end(), TempVertice);
		VBuffer = new VertexBuffer(TempVertice, Vertice.size(), sizeof(VertexColor));

		
		Vertice2.push_back(VertexType(Vector(+0, +1, +0), Color::Yellow));
		Vertice2.push_back(VertexType(Vector(-1, +0, +0), Color::Yellow));
		Vertice2.push_back(VertexType(Vector(+1, +0, +0), Color::Yellow));
		
		VertexType * TempVertice2 = new VertexType[Vertice2.size()];
		copy(Vertice2.begin(), Vertice2.end(), TempVertice2);
		
		VBuffer2 = new VertexBuffer(TempVertice2, Vertice2.size(), sizeof(VertexType));
	}

	void TerrainDemo::Destroy()
	{
		SAFE_DELETE(Drawer);
		SAFE_DELETE(Drawer2);
		SAFE_DELETE(VBuffer);
		SAFE_DELETE(VBuffer2);
		SAFE_DELETE(Landscape);
	}

	void TerrainDemo::Tick()
	{
		Landscape->Tick();

		Matrix rotation = Matrix::CreateRotationX(Math::ToRadians(180));
	

		if (Keyboard::Get()->IsPressed(VK_RIGHT))
			Position.X += 20 * SystemTimer::Get()->GetDeltaTime();
		if (Keyboard::Get()->IsPressed(VK_LEFT))
			Position.X -= 20 * SystemTimer::Get()->GetDeltaTime();

		if (Keyboard::Get()->IsPressed(VK_UP))
			Position.Z += 20 * SystemTimer::Get()->GetDeltaTime();
		if (Keyboard::Get()->IsPressed(VK_DOWN))
			Position.Z -= 20 * SystemTimer::Get()->GetDeltaTime();

	
		Landscape->GetPositionY(Position);
		Position.Y += 1.0f;

		Matrix translation = Matrix::CreateTranslation(Position);

		WorldMatrix = rotation * translation;
		Drawer->AsMatrix("World")->SetMatrix(Matrix::Identity);
		Drawer->AsMatrix("View")->SetMatrix(Context::Get()->GetViewMatrix());
		Drawer->AsMatrix("Projection")->SetMatrix(Context::Get()->GetProjectionMatrix());
		Drawer2->AsMatrix("World")->SetMatrix(WorldMatrix);
		Drawer2->AsMatrix("View")->SetMatrix(Context::Get()->GetViewMatrix());
		Drawer2->AsMatrix("Projection")->SetMatrix(Context::Get()->GetProjectionMatrix());
		
	}

	void TerrainDemo::Render()
	{
		Landscape->SetPass(0);
		Landscape->Render();

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		VBuffer->BindToGPU();
		Drawer->Draw(0,0,Vertice.size());
		
		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		VBuffer2->BindToGPU();
		Drawer2->Draw(0,0,Vertice2.size());
	}
}
