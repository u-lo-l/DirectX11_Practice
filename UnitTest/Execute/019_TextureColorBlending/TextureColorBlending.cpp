#include "Pch.h"
#include "TextureColorBlending.h"

namespace Sdt
{
	const UINT TextureColorBlending::VertexCount = 4;
	const UINT TextureColorBlending::IndexCount = 6;

	void TextureColorBlending::Initialize()
	{
		ID3D11Device* Device = D3D::Get()->GetDevice();
		HRESULT Hr;
		Drawer = new HlslShader<VertexType>(L"12_Texture.hlsl");

		// Vertex Buffer
		{
			Vertices.resize(VertexCount);
			Vertices[0].Position = { -0.5f, -0.5f, +0.0f };
			Vertices[1].Position = { -0.5f, +0.5f, +0.0f };
			Vertices[2].Position = { +0.5f, -0.5f, +0.0f };
			Vertices[3].Position = { +0.5f, +0.5f, +0.0f };

			Vertices[0].UV = { -1, +2 };
			Vertices[1].UV = { -1, -1 };
			Vertices[2].UV = { +2, +2 };
			Vertices[3].UV = { +2, -1 };

			Vertices[0].Color = Color::Blue;
			Vertices[1].Color = Color::Blue;
			Vertices[2].Color = Color::Blue;
			Vertices[3].Color = Color::Blue;

			D3D11_BUFFER_DESC BufferDesc;
			ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.ByteWidth = sizeof(VertexType) * VertexCount;
			BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			BufferDesc.CPUAccessFlags = 0;
			BufferDesc.MiscFlags = 0;
			
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
			InitData.pSysMem = Vertices.data();
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			
			Hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
			CHECK(Hr >= 0);
		}
		// Vertex Buffer End

		// Index Buffer
		{
			D3D11_BUFFER_DESC BufferDesc;
			Indices =
			{
				0, 1, 2, 2, 1, 3
			};

			ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.ByteWidth = sizeof(IndexType) * IndexCount;
			BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
			InitData.pSysMem = Indices.data();
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			Hr = Device->CreateBuffer(&BufferDesc, &InitData, &IndexBuffer);
			CHECK(Hr >= 0);
		}
		// Index Buffer End

		// SAFE_DELETE_ARR(Indices)
		// SAFE_DELETE_ARR(Vertices)

		{
			WorldMat = Matrix::Identity;
			Context::Get()->GetCamera()->SetPosition( 0, 0, 2 );
			Context::Get()->GetCamera()->SetRotation( 180, 0, 180);
			
			D3D11_BUFFER_DESC ConstantBufferDesc = {};
			ConstantBufferDesc.ByteWidth = sizeof(Matrix) * 3;
			ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			CHECK(Device->CreateBuffer(&ConstantBufferDesc, nullptr, &WVPCBuffer) >= 0);
		}


		{
			D3D11_BUFFER_DESC LerpRateConstantBufferDesc = {};
			LerpRateConstantBufferDesc.ByteWidth = sizeof(float) * 4;
			LerpRateConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			LerpRateConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			LerpRateConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			CHECK(Device->CreateBuffer(&LerpRateConstantBufferDesc, nullptr, &LerpRateCBuffer) >= 0);
		}

		{
			SampleTexture = new Texture(wstring(W_TEXTURE_PATH) + L"ChromeIcon.png");
		}
		
		{
			D3D11_RASTERIZER_DESC RasterizerDesc;
			RasterizerDesc.FillMode = D3D11_FILL_SOLID;
			RasterizerDesc.CullMode = D3D11_CULL_NONE;
			RasterizerDesc.FrontCounterClockwise = true;
			RasterizerDesc.DepthBias = 0;
			RasterizerDesc.DepthBiasClamp = 0.0f;
			RasterizerDesc.SlopeScaledDepthBias = 0.0f;
			RasterizerDesc.DepthClipEnable = true;
			RasterizerDesc.ScissorEnable = false;
			RasterizerDesc.MultisampleEnable = false;
			RasterizerDesc.AntialiasedLineEnable = false;
			CHECK(Drawer->CreateRasterizerState(&RasterizerDesc) >= 0);
		}

		//Sampler
		{
			D3D11_SAMPLER_DESC SamplerDesc = {};
			SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			
			SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;  // 주소 모드 설정 (기본값: 반복)
			SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			
			SamplerDesc.BorderColor[0] = 1.f;
			SamplerDesc.BorderColor[1] = 1.f;
			SamplerDesc.BorderColor[2] = 1.f;
			SamplerDesc.BorderColor[3] = 1.f;
			
			SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // 비교 함수 설정 (기본값: 사용 안함)
			SamplerDesc.MinLOD = 0;
			SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			CHECK(D3D::Get()->GetDevice()->CreateSamplerState(&SamplerDesc, &SamplerState) >= 0);
			D3D::Get()->GetDeviceContext()->PSSetSamplers(0, 1, &SamplerState);
		}
	}

	void TextureColorBlending::Destroy()
	{
		SAFE_RELEASE(IndexBuffer)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(Drawer)
		SAFE_RELEASE(SamplerState);
		SAFE_RELEASE(WVPCBuffer);
		SAFE_RELEASE(LerpRateCBuffer);
		SAFE_DELETE(SampleTexture);
	}

	void TextureColorBlending::Tick()
	{
		ImGui::SliderFloat("LerpRate", &LerpRate, 0, 1);
		ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
		
		// Constant Buffer
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		if (SUCCEEDED(DeviceContext->Map(WVPCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
		{
			struct Temp
			{
				Matrix World;
				Matrix View;
				Matrix Projection;
			} BufferData = {
				WorldMat,
				Context::Get()->GetViewMatrix(),
				Context::Get()->GetProjectionMatrix()
			};
			memcpy(MappedResource.pData, &BufferData, sizeof(Temp));
			DeviceContext->Unmap(WVPCBuffer, 0);
		}
		D3D11_MAPPED_SUBRESOURCE MappedResource2;
		if (SUCCEEDED(DeviceContext->Map(LerpRateCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource2)))
		{
			struct Temp
			{
				float Rate;
				float Padding[3];
			} LerpBuffer { LerpRate, {} };
			memcpy(MappedResource2.pData, &LerpBuffer, sizeof(Temp));
			DeviceContext->Unmap(LerpRateCBuffer, 0);			
		}
		// Constant Buffer
	}

	void TextureColorBlending::Render()
	{
		ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
		constexpr UINT stride = sizeof(VertexType);
		constexpr UINT offset = 0;

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		DeviceContext->VSSetConstantBuffers(0, 1, &WVPCBuffer);
		DeviceContext->PSSetConstantBuffers(1, 1, &LerpRateCBuffer);
		DeviceContext->PSSetShaderResources(0, 1, *SampleTexture);
		
		Drawer->DrawIndexed(IndexCount);
	}
}