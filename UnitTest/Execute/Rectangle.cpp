#include "Pch.h"
#include "Rectangle.h"

#include <cmath>
namespace Sdt
{
	const int Rectangle::VERTEX_COUNT = 6;

	void Rectangle::Initialize()
	{
		shader = new Shader(L"06_Rectangle.fx");

		Vertices = new InnerVertexType[VERTEX_COUNT];
		Vertices[0] = { +0.0f, +0.0f, +0.0f };
		Vertices[1] = { +0.0f, +0.5f, +0.0f };
		Vertices[2] = { +0.5f, +0.0f, +0.0f };

		Vertices[3] = { +0.0f, +0.0f, +0.0f };
		Vertices[4] = { +0.0f, +0.5f, +0.0f };
		Vertices[5] = { +0.5f, +0.0f, +0.0f };


		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(InnerVertexType) * VERTEX_COUNT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource;
		ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));
		subResource.pSysMem = Vertices;

		const HRESULT hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, &VertexBuffer);
		CHECK(hr)
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

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		shader->Draw(0, 0, VERTEX_COUNT);

		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		shader->Draw(0, 1, VERTEX_COUNT);
	}
}