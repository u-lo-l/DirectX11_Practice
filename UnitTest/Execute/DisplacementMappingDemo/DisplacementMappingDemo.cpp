#include "Pch.h"
#include "DisplacementMappingDemo.h"
namespace sdt
{
	void DisplacementMappingDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 150, 150, 20 );
		MainCamera->SetRotation( 225, 0, 180);
		
		DisplacementMappingShader = new HlslShader<VertexType>(
			L"Tessellation/DisplacementMap.hlsl",
			static_cast<UINT>(ShaderType::VHDP)
		);
		DisplacementMappingShader->SetTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		DisplacementMappingShader->CreateBlendState_Opaque();
		DisplacementMappingShader->CreateRasterizerState_WireFrame_NoCull();
		DisplacementMappingShader->CreateDepthStencilState_Default();
		DisplacementMappingShader->CreateSamplerState_Linear_Clamp();
		
		HeightMap = new Texture(L"DisplacementTest.png", true);
		float scaler = 2;
		const UINT Width = static_cast<UINT>(HeightMap->GetWidth() / scaler);
		const UINT Height = static_cast<UINT>(HeightMap->GetHeight() / scaler);
		float du = 1 / static_cast<float>(Width);
		float dv = 1 / static_cast<float>(Height);
		
		UINT VertexCount = Width * Height;
		Vertices.resize(VertexCount);
		for (UINT Z = 0 ; Z < Height ; Z++)
		{
			for (UINT X = 0 ; X < Width ; X++)
			{
				const UINT Index = Z * Width + X;
				Vertices[Index].Position.X = static_cast<float>(X) * scaler;
				Vertices[Index].Position.Y = 0;
				Vertices[Index].Position.Z = static_cast<float>(Z) * scaler;
				Vertices[Index].Normal = {0,1,0};
				Vertices[Index].UV = {du * X ,dv * Z };
			}
		}
		UINT IndexCount = (Width - 1) * ( Height - 1) * 6;
		Indices.resize(IndexCount);

		UINT Index = 0;
		for (UINT Z = 0 ; Z <  Height - 1 ; Z++)
		{
			for (UINT X = 0 ; X < Width  - 1; X++)
			{
				Indices[Index + 0] = Width * Z + X;
				Indices[Index + 1] = Width * (Z + 1) + X;
				Indices[Index + 2] = Width * Z + (X + 1);
				Indices[Index + 3] = Width * Z + (X + 1);
				Indices[Index + 4] = Width * (Z + 1) + X;
				Indices[Index + 5] = Width * (Z + 1) + (X + 1);
				Index += 6;
			}
		}
		VBuffer = new VertexBuffer(Vertices.data(), Vertices.size(), sizeof(VertexType));
		IBuffer = new IndexBuffer(Indices.data(), Indices.size());
		WVPBuffer_DS = new ConstantBuffer(
			(UINT)ShaderType::VHDP,
			0,
			&WVP,
			"World View Projection",
			sizeof(WVPDesc),
			false
		);
		DisplacementBuffer_DS = new ConstantBuffer(
			(UINT)ShaderType::VD,
			1,
			&DisplacementFactor,
			"DisplacementFactor",
			sizeof(DisplacementFactorDesc),
			false
		);
	
		WVP.World = Matrix::Identity;
	}

	void DisplacementMappingDemo::Destroy()
	{
		SAFE_DELETE(DisplacementMappingShader);
		SAFE_DELETE(HeightMap);
		SAFE_DELETE(VBuffer);
		SAFE_DELETE(IBuffer);
		SAFE_DELETE(WVPBuffer_DS);
	}

	void DisplacementMappingDemo::Tick()
	{
		WVP.View = Context::Get()->GetViewMatrix();
		WVP.Projection = Context::Get()->GetProjectionMatrix();
		WVPBuffer_DS->UpdateData(&WVP, sizeof(WVPDesc));

		ImGui::SeparatorText("Displacement Factor");
		ImGui::SliderFloat("Displacement Factor", &DisplacementFactor.Displacement, -64, 64);
		DisplacementBuffer_DS->UpdateData(&DisplacementFactor, sizeof(DisplacementFactorDesc));

	}

	void DisplacementMappingDemo::Render()
	{
		VBuffer->BindToGPU();
		IBuffer->BindToGPU();
		HeightMap->BindToGPU(0, (UINT)ShaderType::VertexShader | (UINT)ShaderType::DomainShader);
		WVPBuffer_DS->BindToGPU();
		DisplacementBuffer_DS->BindToGPU();
		DisplacementMappingShader->DrawIndexed(Indices.size());
	}
}
