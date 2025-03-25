#include "Pch.h"
#include "Triangle_List.h"

#include <cmath>

void Triangle_List::Initialize()
{
	// shader = new Shader(L"05_Triangle_List.fx");
	Drawer = new HlslShader<VertexType>(L"05_Triangle_List.hlsl");

	{
		Vertices.emplace_back(Vector(-0.3f, 0.0f, +0.0f), Color(1, 1, 0, 1));
		Vertices.emplace_back(Vector(+0.0f, +0.5f, +0.0f), Color(0, 1, 1, 1));
		Vertices.emplace_back(Vector(+0.3f, 0.0f, +0.0f), Color(1, 0, 1, 1));

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexType) * Vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource;
		ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));

		VertexType* temp = new VertexType[Vertices.size()];
		copy(Vertices.begin(), Vertices.end(), temp);
		subResource.pSysMem = temp;

		const HRESULT hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, &VertexBuffer);
		SAFE_DELETE_ARR(temp)
		CHECK(hr >= 0);
	}
	Vertices.clear();

	//VertexBuffer2
	{
		Vertices.emplace_back(Vector(-0.3f, +0.0f, +0.0f), Color(1, 1, 0, 1));
		Vertices.emplace_back(Vector(+0.3f, +0.0f, +0.0f), Color(1, 0, 1, 1));
		Vertices.emplace_back(Vector(+0.0f, -0.5f, +0.0f), Color(0, 1, 1, 1));

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexType) * Vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource;
		ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));

		VertexType* temp = new VertexType[Vertices.size()];
		copy(Vertices.begin(), Vertices.end(), temp);
		subResource.pSysMem = temp;

		const HRESULT hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, &VertexBuffer2);
		SAFE_DELETE_ARR(temp)
		CHECK(hr >= 0);
	}
}

void Triangle_List::Destroy()
{
	SAFE_RELEASE(VertexBuffer)
	// SAFE_DELETE(shader)
	SAFE_DELETE(Drawer)
}

void Triangle_List::Tick()
{

}

void Triangle_List::Render()
{
	constexpr UINT stride = sizeof(VertexType);
	constexpr UINT offset = 0;

	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	Drawer->Draw(Vertices.size());
	// shader->Draw(0, 0, Vertices.size());
	//
	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer2, &stride, &offset);
	Drawer->Draw(Vertices.size());
	// shader->Draw(0, 2, Vertices.size());
}
