#include "Pch.h"
#include "WorldDemo.h"

namespace Sdt
{
	const int WorldDemo::VERTEX_COUNT = 3;

	void WorldDemo::Initialize()
	{
		shader = new Shader(L"09_World.fx");

		Vertices = new InnerVertexType[VERTEX_COUNT];

		Vertices[0].Position = Vector(+0.5f, +0.0f, +0.0f);
		Vertices[1].Position = Vector(+0.0f, +0.5f, +0.0f);
		Vertices[2].Position = Vector(+0.5f, +0.5f, +0.0f);

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

		D3DXMatrixIdentity(&WorldMat);

		Vector CameraLocation{ 0, 0, -10 };
		Vector CameraForward{ 0, 0, 1 };
		Vector CameraUp{ 0, 1, 0 };
		Vector CameraAt = CameraLocation + CameraForward;
		D3DXMatrixLookAtLH(&ViewMat, &CameraLocation, &CameraAt, &CameraUp);

		float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
		D3DXMatrixPerspectiveFovLH(&ProjectionMat, Math::PI * 0.25f, Aspect, 0.1f, 1000);
	}

	void WorldDemo::Destroy()
	{
		SAFE_DELETE_ARR(Vertices)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(shader)
	}

	void WorldDemo::Tick()
	{
		static Vector2D Location {0, 0};
		ImGui::SliderFloat2("Location", Location, -5.f, +5.f, "%.2f");

		if (Keyboard::Get()->IsPressed(VK_RIGHT))
		{
			WorldMat._11 += 0.001f;
			WorldMat._22 += 0.001f;
		}
		if (Keyboard::Get()->IsPressed(VK_LEFT))
		{
			WorldMat._11 -= 0.001f;
			WorldMat._22 -= 0.001f;
		}

		WorldMat._41 = Location.x;
		WorldMat._42 = Location.y;
	}

	void WorldDemo::Render()
	{
		shader->AsMatrix("World")->SetMatrix(WorldMat);
		shader->AsMatrix("View")->SetMatrix(ViewMat);
		shader->AsMatrix("Projection")->SetMatrix(ProjectionMat);

		constexpr UINT stride = sizeof(InnerVertexType);
		constexpr UINT offset = 0;

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		shader->Draw(0, 0, VERTEX_COUNT);
	}
}