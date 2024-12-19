#include "Pch.h"
#include "Line.h"

void Line::Initialize()
{
	shader = new Shader(L"04_Line.fx");

	Vertices[0] = { -1.0f, -1.0f, 0.0f };
	Vertices[1] = { 1.0f, 1.0f, 0.0f };


	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vector) * 2;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));
	subResource.pSysMem = Vertices;

	HRESULT hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, &VertexBuffer);
	CHECK(hr);
}

void Line::Destroy()
{
	SAFE_RELEASE(VertexBuffer);
	SAFE_DELETE(shader);
}

void Line::Tick()
{

}

void Line::Render()
{
	UINT stride = sizeof(Vector);
	UINT offset = 0;

	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	shader->Draw(0, 0, 2);
}
