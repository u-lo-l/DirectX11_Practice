#include "Pch.h"
#include "WorldDemo.h"

namespace Sdt
{
	const int WorldDemo::VERTEX_COUNT = 3;

	void WorldDemo::Initialize()
	{
		// shader = new Shader(L"09_World.fx");
		Drawer = new HlslShader<VertexType>(L"09_World.hlsl");

		Vertices = new VertexType[VERTEX_COUNT];

		Vertices[0].Position = Vector(+0.5f, +0.0f, 0.f);
		Vertices[1].Position = Vector(+0.0f, +0.5f, 0.f);
		Vertices[2].Position = Vector(+0.5f, +0.5f, 0.f);

		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(VertexType) * VERTEX_COUNT;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitData.pSysMem = &(Vertices[0]);

		ID3D11Device * const Device = D3D::Get()->GetDevice();
		HRESULT hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
		CHECK(hr >= 0);

		WorldMat = Matrix::Identity;

		Vector CameraLocation{ 0, 0, -10.f };
		Vector CameraForward{ 0, 0, 1 };
		Vector CameraUp{ 0, 1, 0 };
		Vector CameraAt = CameraLocation + CameraForward;
		ViewMat = Matrix::CreateLookAt(CameraLocation, CameraAt, CameraUp);
		const float AspectRatio = static_cast<float>(D3D::GetDesc().Width) / static_cast<float>(D3D::GetDesc().Height);
		ProjectionMat = Matrix::CreatePerspectiveFieldOfView(Math::Pi * 0.25f, AspectRatio, 0.01f, 1000.f);
		
		D3D11_BUFFER_DESC ConstantBufferDesc = {};
		ConstantBufferDesc.ByteWidth = sizeof(Matrix);
		ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = Device->CreateBuffer(&ConstantBufferDesc, nullptr, &CB_World);
		hr = Device->CreateBuffer(&ConstantBufferDesc, nullptr, &CB_View);
		hr = Device->CreateBuffer(&ConstantBufferDesc, nullptr, &CB_Projection);

		Drawer->AddConstantBuffer(CB_World);
		Drawer->AddConstantBuffer(CB_View);
		Drawer->AddConstantBuffer(CB_Projection);
	}

	void WorldDemo::Destroy()
	{
		SAFE_DELETE_ARR(Vertices)
		SAFE_RELEASE(VertexBuffer)
		// SAFE_DELETE(shader)
		SAFE_DELETE(Drawer)

		SAFE_RELEASE(CB_World)
		SAFE_RELEASE(CB_View)
		SAFE_RELEASE(CB_Projection)
	}

	void WorldDemo::Tick()
	{
		static Vector2D Location {0, 0};
		ImGui::SliderFloat2("Location", Location, -5.f, +5.f, "%.2f");

		if (Keyboard::Get()->IsPressed(VK_RIGHT))
		{
			WorldMat.M11 += 0.001f;
			WorldMat.M22 += 0.001f;
		}
		if (Keyboard::Get()->IsPressed(VK_LEFT))
		{
			WorldMat.M11 -= 0.001f;
			WorldMat.M22 -= 0.001f;
		}

		WorldMat.M41 = Location.X;
		WorldMat.M42 = Location.Y;
	}

	void WorldDemo::Render()
	{
		ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
		D3D11_MAPPED_SUBRESOURCE MappedResource1;
		if (SUCCEEDED(DeviceContext->Map(CB_World, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource1)))
		{
			memcpy(MappedResource1.pData, &WorldMat, sizeof(Matrix));
			DeviceContext->Unmap(CB_World, 0);
		}
		
		D3D11_MAPPED_SUBRESOURCE MappedResource2;
		if (SUCCEEDED(DeviceContext->Map(CB_View, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource2)))
		{
			memcpy(MappedResource2.pData, &ViewMat, sizeof(Matrix));
			DeviceContext->Unmap(CB_View, 0);
		}

		D3D11_MAPPED_SUBRESOURCE MappedResource3;
		if (SUCCEEDED(DeviceContext->Map(CB_Projection, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource3)))
		{
			memcpy(MappedResource3.pData, &ProjectionMat, sizeof(Matrix));
			DeviceContext->Unmap(CB_Projection, 0);
		}


		constexpr UINT stride = sizeof(VertexType);
		constexpr UINT offset = 0;

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		
		DeviceContext->VSSetConstantBuffers(0, 1, &CB_World);
		DeviceContext->VSSetConstantBuffers(1, 1, &CB_View);
		DeviceContext->VSSetConstantBuffers(2, 1, &CB_Projection);
		// shader->Draw(0, 0, VERTEX_COUNT);
		Drawer->Draw(VERTEX_COUNT);
	}
}