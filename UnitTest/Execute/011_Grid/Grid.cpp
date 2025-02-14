#include "Pch.h"
#include "Grid.h"

namespace Sdt
{
	UINT Grid::INDEX_PER_GRID = 6;

	void Grid::Initialize()
	{
		// shader = new Shader(L"11_Grid.fx");
		Drawer = new HlslShader<VertexType>(L"11_Grid.hlsl");

		// Vertex Buffer
		VertexCount = (Width + 1) * (Height + 1);
		Vertices = new VertexType[VertexCount];

		for (UINT y = 0 ; y <= Height ; y++)
		{
			for ( UINT x = 0 ; x <= Width; x++)
			{
				const UINT Index = (Width + 1) * y + x;
				Vertices[Index].Position.X = static_cast<float>(x) - Width * 0.5f;
				Vertices[Index].Position.Y = static_cast<float>(y) - Height * 0.5f;
				Vertices[Index].Position.Z = 0.f;
			}
		}

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(VertexType) * VertexCount;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = 0;
		BufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitData.pSysMem = Vertices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		ID3D11Device* Device = D3D::Get()->GetDevice();
		HRESULT hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
		CHECK(hr >= 0);
		// Vertex Buffer End

		// Index Buffer
		IndexCount = Width * Height * INDEX_PER_GRID;
		Indices = new UINT[IndexCount];

		for (UINT y = 0, Index = 0; y < Height; y++)
		{
			for (UINT x = 0; x < Width; x++, Index += INDEX_PER_GRID)
			{
				Indices[Index + 0] = (Width + 1) * y + x;
				Indices[Index + 1] = (Width + 1) * (y + 1) + x;
				Indices[Index + 2] = (Width + 1) * y + (x + 1);

				Indices[Index + 3] = (Width + 1) * y + (x + 1);
				Indices[Index + 4] = (Width + 1) * (y + 1) + x;
				Indices[Index + 5] = (Width + 1) * (y + 1) + (x + 1);
			}
		}

		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(IndexType) * IndexCount;
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;


		ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitData.pSysMem = Indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = Device->CreateBuffer(&BufferDesc, &InitData, &IndexBuffer);
		CHECK(hr >= 0);
		// Index Buffer End

		SAFE_DELETE_ARR(Indices)
		SAFE_DELETE_ARR(Vertices)

		WorldMat = Matrix::Identity;

		const Vector CameraLocation{ 0, 0, 10 };
		const Vector CameraForward{ 0, 0, -1 };
		const Vector CameraUp{ 0, 1, 0 };
		const Vector CameraAt = CameraLocation + CameraForward;
		ViewMat = Matrix::CreateLookAt(CameraLocation, CameraAt, CameraUp);

		const float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
		ProjectionMat = Matrix::CreatePerspectiveFieldOfView(Math::Pi * 0.25f, Aspect, 0.01f, 100.0f);

		D3D11_BUFFER_DESC ConstantBufferDesc = {};
		ConstantBufferDesc.ByteWidth = sizeof(Matrix) * 3;
		ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		CHECK(Device->CreateBuffer(&ConstantBufferDesc, nullptr, &WVPCBuffer) >= 0);

		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		RasterizerDesc.FrontCounterClockwise = true;
		RasterizerDesc.DepthBias = 0;
		RasterizerDesc.DepthBiasClamp = 0.0f;
		RasterizerDesc.SlopeScaledDepthBias = 0.0f;
		RasterizerDesc.DepthClipEnable = true;
		RasterizerDesc.ScissorEnable = false;
		RasterizerDesc.MultisampleEnable = false;
		RasterizerDesc.AntialiasedLineEnable = false;
		CHECK(Drawer->SetRasterizerState(&RasterizerDesc) >= 0);
	}

	void Grid::Destroy()
	{
		SAFE_RELEASE(IndexBuffer)
		SAFE_RELEASE(VertexBuffer)
		// SAFE_DELETE(shader)
		SAFE_DELETE(Drawer)
	}

	void Grid::Tick()
	{
		ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
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
				ViewMat,
				ProjectionMat
			};
			memcpy(MappedResource.pData, &BufferData, sizeof(Temp));
			DeviceContext->Unmap(WVPCBuffer, 0);
		}
	}

	void Grid::Render()
	{
		ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
		constexpr UINT stride = sizeof(VertexType);
		constexpr UINT offset = 0;

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		DeviceContext->VSSetConstantBuffers(0, 1, &WVPCBuffer);
		
		Drawer->DrawIndexed(IndexCount);
	}
}