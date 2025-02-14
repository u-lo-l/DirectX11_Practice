#include "Pch.h"
#include "WorldDemo2.h"

namespace Sdt
{
	const int WorldDemo2::VERTEX_COUNT = 6;

	void WorldDemo2::Initialize()
	{
		// shader = new Shader(L"09_World2.fx");
		Drawer = new HlslShader<VertexType>(L"09_World2.hlsl");
		Vertices = new VertexType[VERTEX_COUNT];
		
		Vertices[0].Position = Vector(-0.5f, -0.5f, +0.0f);
		Vertices[1].Position = Vector(-0.5f, +0.5f, +0.0f);
		Vertices[2].Position = Vector(+0.5f, -0.5f, +0.0f);
		Vertices[3].Position = Vector(+0.5f, -0.5f, +0.0f);
		Vertices[4].Position = Vector(-0.5f, +0.5f, +0.0f);
		Vertices[5].Position = Vector(+0.5f, +0.5f, +0.0f);
		
		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(VertexType) * VERTEX_COUNT;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = 0;
		BufferDesc.MiscFlags = 0;
		
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitData.pSysMem = Vertices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		
		ID3D11Device* Device = D3D::Get()->GetDevice();
		const HRESULT hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
		CHECK(hr >= 0);
		
		for (int i = 0 ; i < 3 ; i++)
			WorldMat[i] = Matrix::Identity;
		
		Vector CameraLocation{ 0, 0, -10 };
		Vector CameraForward{ 0, 0, 1 };
		Vector CameraUp{ 0, 1, 0 };
		Vector CameraAt = CameraLocation + CameraForward;
		ViewMat = Matrix::CreateLookAt(CameraLocation, CameraAt, CameraUp);
		
		float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
		ProjectionMat = Matrix::CreatePerspectiveFieldOfView(Math::Pi * 0.25f, Aspect, 0.01f, 1000.f);

		D3D11_BUFFER_DESC ConstantBufferDesc = {};
		ConstantBufferDesc.ByteWidth = sizeof(CBufferData);
		ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Device->CreateBuffer(&ConstantBufferDesc, nullptr, &CBuffer1);
		Drawer->AddConstantBuffer(CBuffer1);
		
		ConstantBufferDesc.ByteWidth = sizeof(CBufferData2);
		Device->CreateBuffer(&ConstantBufferDesc, nullptr, &CBuffer2);
		Drawer->AddConstantBuffer(CBuffer2);
	}

	void WorldDemo2::Destroy()
	{
		SAFE_DELETE_ARR(Vertices)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(Drawer)
	}

	void WorldDemo2::Tick()
	{
		static int index = 0;
		ImGui::InputInt("Select", &index);
		index %= 3;
		
		if (Keyboard::Get()->IsPressed(VK_CONTROL) == true)
		{
			if (Keyboard::Get()->IsPressed(VK_RIGHT))
			{
				(WorldMat + index)->M41 += 0.001f;
			}
			if (Keyboard::Get()->IsPressed(VK_LEFT))
			{
				(WorldMat + index)->M41 -= 0.001f;
			}
		}
		else if (Keyboard::Get()->IsPressed(VK_SHIFT) == true)
		{
			if (Keyboard::Get()->IsPressed(VK_RIGHT))
			{
				(WorldMat + index)->M11 += 0.001f;
			}
			if (Keyboard::Get()->IsPressed(VK_LEFT))
			{
				(WorldMat + index)->M11 -= 0.001f;
			}
		}
		else
		{
			if (Keyboard::Get()->IsPressed(VK_RIGHT))
			{
				(WorldMat + index)->M22 += 0.001f;
			}
			if (Keyboard::Get()->IsPressed(VK_LEFT))
			{
				(WorldMat + index)->M22 -= 0.001f;
			}
		}
	}

	void WorldDemo2::Render()
	{
		ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
		D3D11_MAPPED_SUBRESOURCE MappedResource1;
		if (SUCCEEDED(DeviceContext->Map(CBuffer1, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource1)))
		{
			const CBufferData Temp = { ViewMat, ProjectionMat };
			memcpy(MappedResource1.pData, &Temp, sizeof(CBufferData));
			DeviceContext->Unmap(CBuffer1, 0);
		}
		constexpr UINT stride = sizeof(VertexType);
		constexpr UINT offset = 0;
		
		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		
		D3D11_MAPPED_SUBRESOURCE MappedResource2;
		if (SUCCEEDED(DeviceContext->Map(CBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource2)))
		{
			const CBufferData2 Temp = { WorldMat[0], 0, {} };
			memcpy(MappedResource2.pData, &Temp, sizeof(CBufferData2));
			DeviceContext->Unmap(CBuffer2, 0);
			DeviceContext->VSSetConstantBuffers(1,1,&CBuffer2);
			DeviceContext->PSSetConstantBuffers(1,1, &CBuffer2);
		}
		Drawer->Draw(VERTEX_COUNT);
		
		D3D11_MAPPED_SUBRESOURCE MappedResource3;
		if (SUCCEEDED(DeviceContext->Map(CBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource3)))
		{
			const CBufferData2 Temp = { WorldMat[1], 1, {} };
			memcpy(MappedResource3.pData, &Temp, sizeof(CBufferData2));
			DeviceContext->Unmap(CBuffer2, 0);
			DeviceContext->VSSetConstantBuffers(1,1,&CBuffer2);
			DeviceContext->PSSetConstantBuffers(1,1, &CBuffer2);
		}
		Drawer->Draw(VERTEX_COUNT);

		D3D11_MAPPED_SUBRESOURCE MappedResource4;
		if (SUCCEEDED(DeviceContext->Map(CBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource4)))
		{
			const CBufferData2 Temp = { WorldMat[2], 2.0f, {} };
			memcpy(MappedResource4.pData, &Temp, sizeof(CBufferData2));
			DeviceContext->Unmap(CBuffer2, 0);
			DeviceContext->VSSetConstantBuffers(1,1, &CBuffer2);
			DeviceContext->PSSetConstantBuffers(1,1, &CBuffer2);
		}
		Drawer->Draw(VERTEX_COUNT);
	}
}