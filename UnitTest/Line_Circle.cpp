#include "Pch.h"
#include "Line_Circle.h"

void Line_Circle::Initialize()
{
	shader = new Shader(L"04_Line_Circle.fx");

	Vertices.emplace_back(Vector(0.f, +0.5f, 0.f), Color(1,0,0,1));
	Vertices.emplace_back(Vector(1.f, +0.5f, 0.f), Color(1,0,0,1));

	Vertices.emplace_back(Vector(0.f, 0.f, 0.f), Color(0,1,0,1));
	Vertices.emplace_back(Vector(1.f, 0.f, 0.f), Color(0,1,0,1));

	Vertices.emplace_back(Vector(0.f, -0.5f, 0.f), Color(0,0,1,1));
	Vertices.emplace_back(Vector(1.f, -0.5f, 0.f), Color(0,0,1,1));


	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(InnerVertexType) * Vertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));

	InnerVertexType * temp = new InnerVertexType[Vertices.size()];
	copy(Vertices.begin(), Vertices.end(), temp);
	subResource.pSysMem = temp;

	const HRESULT hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, &VertexBuffer);

	SAFE_DELETE_ARR(temp)
	CHECK(hr)
}

void Line_Circle::Destroy()
{
	SAFE_RELEASE(VertexBuffer)
	SAFE_DELETE(shader)
}

void Line_Circle::Tick()
{

}

void Line_Circle::Render()
{
	constexpr UINT stride = sizeof(InnerVertexType);
	constexpr UINT offset = 0;

	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	shader->Draw(0, 0, Vertices.size());
}
