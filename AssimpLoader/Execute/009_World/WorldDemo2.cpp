#include "Pch.h"
#include "WorldDemo2.h"

namespace Sdt
{
	const int WorldDemo2::VERTEX_COUNT = 6;

	void WorldDemo2::Initialize()
	{
		shader = new Shader(L"09_World2.fx");

		Vertices = new InnerVertexType[VERTEX_COUNT];

		Vertices[0].Position = Vector(-0.5f, -0.5f, +0.0f);
		Vertices[1].Position = Vector(-0.5f, +0.5f, +0.0f);
		Vertices[2].Position = Vector(+0.5f, -0.5f, +0.0f);
		Vertices[3].Position = Vector(+0.5f, -0.5f, +0.0f);
		Vertices[4].Position = Vector(-0.5f, +0.5f, +0.0f);
		Vertices[5].Position = Vector(+0.5f, +0.5f, +0.0f);

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(InnerVertexType) * VERTEX_COUNT;
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
		CHECK(hr);

		for (int i = 0 ; i < 3 ; i++)
			D3DXMatrixIdentity(WorldMat + i);

		Vector CameraLocation{ 0, 0, -10 };
		Vector CameraForward{ 0, 0, 1 };
		Vector CameraUp{ 0, 1, 0 };
		Vector CameraAt = CameraLocation + CameraForward;
		D3DXMatrixLookAtLH(&ViewMat, &CameraLocation, &CameraAt, &CameraUp);

		float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
		D3DXMatrixPerspectiveFovLH(&ProjectionMat, Math::PI * 0.25f, Aspect, 0.1f, 1000);
	}

	void WorldDemo2::Destroy()
	{
		SAFE_DELETE_ARR(Vertices)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(shader)
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
				(WorldMat + index)->_41 += 0.001f;
			}
			if (Keyboard::Get()->IsPressed(VK_LEFT))
			{
				(WorldMat + index)->_41 -= 0.001f;
			}
		}
		else if (Keyboard::Get()->IsPressed(VK_SHIFT) == true)
		{
			if (Keyboard::Get()->IsPressed(VK_RIGHT))
			{
				(WorldMat + index)->_11 += 0.001f;
			}
			if (Keyboard::Get()->IsPressed(VK_LEFT))
			{
				(WorldMat + index)->_11 -= 0.001f;
			}
		}
		else
		{
			if (Keyboard::Get()->IsPressed(VK_RIGHT))
			{
				(WorldMat + index)->_22 += 0.001f;
			}
			if (Keyboard::Get()->IsPressed(VK_LEFT))
			{
				(WorldMat + index)->_22 -= 0.001f;
			}
		}

	}

	void WorldDemo2::Render()
	{
		shader->AsMatrix("View")->SetMatrix(ViewMat);
		shader->AsMatrix("Projection")->SetMatrix(ProjectionMat);

		constexpr UINT stride = sizeof(InnerVertexType);
		constexpr UINT offset = 0;

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

		shader->AsScalar("Index")->SetInt(0);
		shader->AsMatrix("World")->SetMatrix(WorldMat[0]);
		shader->Draw(0, 0, VERTEX_COUNT);

		shader->AsScalar("Index")->SetInt(1);
		shader->AsMatrix("World")->SetMatrix(WorldMat[1]);
		shader->Draw(0, 0, VERTEX_COUNT);

		shader->AsScalar("Index")->SetInt(2);
		shader->AsMatrix("World")->SetMatrix(WorldMat[2]);
		shader->Draw(0, 0, VERTEX_COUNT);
	}
}