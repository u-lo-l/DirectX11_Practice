#include "Pch.h"
#include "Renders/VertexLayout.h"
#include "NonInstancing_Vertex.h"

namespace Sdt
{
	const wstring ShaderName = L"37_NonInstancing_Vertex.fx";
	
	void NonInstancing_Vertex::Initialize()
	{
		Context::Get()->GetCamera()->SetPosition({ 0,0,-20 });

		Drawer = new Shader(ShaderName);
		PerFrame = new ConstantDataBinder(Drawer);

		DiffuseMap = new Texture( L"Box_Diffuse.png", nullptr, true);
		sDiffuseMap = Drawer->AsSRV("Texture");
		
		sDiffuseMap->SetResource(DiffuseMap->GetSRV());
		
		for (UINT i = 0; i < MaxInstanceCount; i++)
		{
			Transforms[i] = new Transform();
			Transforms[i]->SetPosition(Vector::Random(-300, 300));
			Transforms[i]->SetScale(Vector::Random(1, 2));
			Transforms[i]->SetRotation(Vector::Random(-180, 180));
			Transforms[i]->Tick();
		}
		CreateCube();
	}

	void NonInstancing_Vertex::Destroy()
	{
		SAFE_DELETE(Drawer);
	}

	void NonInstancing_Vertex::Tick()
	{
		PerFrame->Tick();
	}

	void NonInstancing_Vertex::Render()
	{
		PerFrame->BindToGPU();

		VBuffer->BindToGPU();
		IBuffer->BindToGPU();

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
		for (Transform * TF : Transforms)
		{
			TF->BindCBufferToGPU(Drawer);
			Drawer->DrawIndexed(0, 0, Indices.size());
		}
	}

	void NonInstancing_Vertex::CreateCube(float InSize)
	{
		const float Width = InSize * 0.5f;
		const float Height = InSize * 0.5f;
		const float Depth = InSize * 0.5f;

		// Front
		Vertices.emplace_back(VertexType({-Width, -Height, -Depth}, {0,1}, {0,0,-1}));
		Vertices.emplace_back(VertexType({-Width, Height, -Depth}, {0,0}, {0,0,-1}));
		Vertices.emplace_back(VertexType({Width, Height, -Depth}, {1,0}, {0,0,-1}));
		Vertices.emplace_back(VertexType({Width, -Height, -Depth}, {1,1}, {0,0,-1}));
		// Back
		Vertices.emplace_back(VertexType({-Width, -Height, +Depth}, {1,1}, {0,0,1}));
		Vertices.emplace_back(VertexType({+Width, -Height, +Depth}, {0,1}, {0,0,1}));
		Vertices.emplace_back(VertexType({+Width, Height, +Depth}, {0,0}, {0,0,1}));
		Vertices.emplace_back(VertexType({+Width, Height, +Depth}, {1,0}, {0,0,1}));
		
		// Top
		Vertices.emplace_back(VertexType({-Width, +Height, -Depth}, {0,1}, {0,1,0}));
		Vertices.emplace_back(VertexType({-Width, +Height, +Depth}, {0,0}, {0,1,0}));
		Vertices.emplace_back(VertexType({+Width, +Height, +Depth}, {1,0}, {0,1,0}));
		Vertices.emplace_back(VertexType({+Width, Height, -Depth}, {1,1}, {0,1,0}));
		// Bottom
		Vertices.emplace_back(VertexType({-Width, -Height, -Depth}, {1,1}, {0,-1,0}));
		Vertices.emplace_back(VertexType({+Width, -Height, -Depth}, {0,1}, {0,-1,0}));
		Vertices.emplace_back(VertexType({+Width, -Height, +Depth}, {1,0}, {0,-1,0}));
		Vertices.emplace_back(VertexType({-Width, -Height, +Depth}, {1,0}, {0,-1,0}));
		
		// Left
		Vertices.emplace_back(VertexType({-Width, -Height, +Depth}, {0, 1}, {-1, 0, 0}));
		Vertices.emplace_back(VertexType({-Width, +Height, +Depth}, {0, 0}, {-1, 0, 0}));
		Vertices.emplace_back(VertexType({-Width, +Height, -Depth}, {1, 0}, {-1, 0, 0}));
		Vertices.emplace_back(VertexType({-Width, -Height, -Depth}, {1, 1}, {-1, 0, 0}));
		//Right
		Vertices.emplace_back(VertexType({Width, -Height, -Depth}, {0, 1}, {1, 0, 0}));
		Vertices.emplace_back(VertexType({Width, +Height, -Depth}, {0, 0}, {1, 0, 0}));
		Vertices.emplace_back(VertexType({Width, +Height, +Depth}, {1, 0}, {1, 0, 0}));
		Vertices.emplace_back(VertexType({Width, -Height, +Depth}, {1, 1}, {1, 0, 0}));

		Indices.clear();
		Indices.reserve(36);
		Indices.insert(Indices.end(),
		{
			 0,  1,  2,  0,  2,  3,	// front
			 4,  5,  6,  4,  6,  7,	// back
			 8,  9, 10,  8, 10, 11,	// top
			12, 13, 14, 12, 14, 15,	// bottom
			16, 17, 18, 16, 18, 19,	// left
			20, 21, 22, 20, 22, 23,	// right
		});

		VBuffer = new VertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));
		IBuffer = new IndexBuffer(Indices.data(), Indices.size());
	}
}
