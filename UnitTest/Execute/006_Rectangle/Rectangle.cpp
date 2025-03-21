#include "Pch.h"
#include "Rectangle.h"

namespace Sdt
{
	const int Rectangle::VERTEX_COUNT = 4;

	void Rectangle::Initialize()
	{
		shader = new Shader(L"06_Rectangle.fx");

		Vertices = new InnerVertexType[VERTEX_COUNT];
		Vertices[0] = { +0.0f, +0.0f, +0.0f };
		Vertices[1] = { +0.0f, +0.5f, +0.0f };
		Vertices[2] = { +0.5f, +0.0f, +0.0f };
		Vertices[3] = { +0.5f, +0.5f, +0.0f };

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

		ID3D11Device * Device = D3D::Get()->GetDevice();
		const HRESULT hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
		CHECK(hr >= 0);
	}

	void Rectangle::Destroy()
	{
		SAFE_DELETE_ARR(Vertices)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(shader)
	}

	void Rectangle::Tick()
	{

	}

	void Rectangle::Render()
	{
		constexpr UINT stride = sizeof(InnerVertexType);
		constexpr UINT offset = 0;

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		shader->Draw(0, 0, VERTEX_COUNT);

		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		shader->Draw(0, 1, VERTEX_COUNT);
	}
}