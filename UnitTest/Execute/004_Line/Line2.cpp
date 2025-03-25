#include "Pch.h"
#include "Line2.h"

void Line2::Initialize()
{
	// shader = new Shader(L"04_Line2.fx");
	hlslShader = new HlslShader<VertexType>(L"04_Line2.hlsl");
	
	
	Vertices.emplace_back(Vector(-0.5f, +0.5f, 0.f), Color(1,0,0,1));
	Vertices.emplace_back(Vector(0.5f, +0.5f, 0.f), Color(1,0,0,1));

	Vertices.emplace_back(Vector(-0.5f, 0.f, 0.f), Color(0,1,0,1));
	Vertices.emplace_back(Vector(0.5f, 0.f, 0.f), Color(0,1,0,1));

	Vertices.emplace_back(Vector(-0.5f, -0.5f, 0.f), Color(0,0,1,1));
	Vertices.emplace_back(Vector(0.5f, -0.5f, 0.f), Color(0,0,1,1));


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

void Line2::Destroy()
{
	SAFE_RELEASE(VertexBuffer)
	SAFE_DELETE(shader)
	SAFE_DELETE(hlslShader)
}

void Line2::Tick()
{

}

void Line2::Render()
{
	constexpr UINT stride = sizeof(VertexType);
	constexpr UINT offset = 0;

	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	// D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// shader->Draw(0, 0, Vertices.size());
	hlslShader->Draw(Vertices.size());
}
