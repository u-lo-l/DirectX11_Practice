#include "Pch.h"
#include "Renders/VertexLayout.h"
#include "Instancing_Vertex.h"

namespace Sdt
{
	const wstring ShaderName = L"37_Instancing_Vertex.fx";
	
	void Instancing_Vertex::Initialize()
	{
		Context::Get()->GetCamera()->SetPosition({ 0,0,0 });

		Drawer = new Shader(ShaderName);
		PerFrame = new ConstantDataBinder(Drawer);

		DiffuseMap = new Texture( L"Box_Diffuse.png",  true);
		sDiffuseMap = Drawer->AsSRV("Texture");
		
		sDiffuseMap->SetResource(DiffuseMap->GetSRV());
		
		for (UINT i = 0; i < MaxInstanceCount; i++)
		{
			Transforms[i] = new Transform();
			Transforms[i]->SetPosition(Vector::Random(20.f, 60.f));
			const float Scale = Math::Random(1.f ,2.f);
			Transforms[i]->SetScale({Scale, Scale, Scale});
			Transforms[i]->UpdateWorldMatrix();

			Worlds[i] = Transforms[i]->GetMatrix();
		}
		CreateCube();

		InstanceBuffer = new VertexBuffer(Worlds, MaxInstanceCount, sizeof(Matrix), Shader::InstancingSlot);
	}

	void Instancing_Vertex::Destroy()
	{
		SAFE_DELETE(Drawer);
	}

	void Instancing_Vertex::Tick()
	{
		PerFrame->Tick();
	}

	void Instancing_Vertex::Render()
	{
		PerFrame->BindToGPU();

		VBuffer->BindToGPU();
		InstanceBuffer->BindToGPU();
		
		IBuffer->BindToGPU();

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Drawer->DrawIndexedInstanced(0, 0, Indices.size(), MaxInstanceCount);
	}

	void Instancing_Vertex::CreateCube(float InSize)
	{
		const float Width = InSize * 0.5f;
		const float Height = InSize * 0.5f;
		const float Depth = InSize * 0.5f;

		//Front
		Vertices.push_back(VertexTextureNormal({-Width, -Height, -Depth}, {0, 1}, {0, 0, -1}));
		Vertices.push_back(VertexTextureNormal({-Width, +Height, -Depth}, {0, 0}, {0, 0, -1}));
		Vertices.push_back(VertexTextureNormal({+Width, +Height, -Depth}, {1, 0}, {0, 0, -1}));
		Vertices.push_back(VertexTextureNormal({+Width, -Height, -Depth}, {1, 1}, {1, 0, -1}));

		//Back
		Vertices.push_back(VertexTextureNormal({-Width, -Height, +Depth}, {1, 1}, {0, 0, +1}));
		Vertices.push_back(VertexTextureNormal({+Width, -Height, +Depth}, {0, 1}, {0, 0, +1}));
		Vertices.push_back(VertexTextureNormal({+Width, +Height, +Depth}, {0, 0}, {0, 0, +1}));
		Vertices.push_back(VertexTextureNormal({-Width, +Height, +Depth}, {1, 0}, {0, 0, +1}));

		//Top
		Vertices.push_back(VertexTextureNormal({-Width, +Height, -Depth}, {0, 1}, { 0, 1, 0}));
		Vertices.push_back(VertexTextureNormal({-Width, +Height, +Depth}, {0, 0}, { 0, 1, 0}));
		Vertices.push_back(VertexTextureNormal({+Width, +Height, +Depth}, {1, 0}, { 0, 1, 0}));
		Vertices.push_back(VertexTextureNormal({+Width, +Height, -Depth}, {1, 1}, { 0, 1, 0}));

		// Bottom
		Vertices.push_back(VertexTextureNormal({-Width, -Height, -Depth}, {1, 1}, {0, -1, 0}));
		Vertices.push_back(VertexTextureNormal({+Width, -Height, -Depth}, {0, 1}, {0, -1, 0}));
		Vertices.push_back(VertexTextureNormal({+Width, -Height, +Depth}, {0, 0}, {0, -1, 0}));
		Vertices.push_back(VertexTextureNormal({-Width, -Height, +Depth}, {1, 0}, {0, -1, 0}));

		//Left
		Vertices.push_back(VertexTextureNormal({-Width, -Height, +Depth}, {0, 1}, {-1, 0, 0}));
		Vertices.push_back(VertexTextureNormal({-Width, +Height, +Depth}, {0, 0}, {-1, 0, 0}));
		Vertices.push_back(VertexTextureNormal({-Width, +Height, -Depth}, {1, 0}, {-1, 0, 0}));
		Vertices.push_back(VertexTextureNormal({-Width, -Height, -Depth}, {1, 1}, {-1, 0, 0}));

		//Right
		Vertices.push_back(VertexTextureNormal({+Width, -Height, -Depth}, {0, 1}, { 1, 0, 0}));
		Vertices.push_back(VertexTextureNormal({+Width, +Height, -Depth}, {0, 0}, { 1, 0, 0}));
		Vertices.push_back(VertexTextureNormal({+Width, +Height, +Depth}, {1, 0}, { 1, 0, 0}));
		Vertices.push_back(VertexTextureNormal({+Width, -Height, +Depth}, {1, 1}, { 1, 0, 0}));

		Indices.clear();
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
