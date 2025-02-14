#include "Pch.h"
#include "Line.h"

void Line::Initialize()
{
	// shader = new Shader(L"04_Line.fx");
	Drawer = new HlslShader<Vertex>(L"04_Line.hlsl");

	Vertices[0].Position = { -1.f, -0.5f, 0 };
	Vertices[1].Position = { 1.f, 0.5f, 0 };

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(Vertex) * 2;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = Vertices;

	HRESULT hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, &VertexBuffer);
	CHECK(hr >= 0);
}

void Line::Destroy()
{
	SAFE_RELEASE(VertexBuffer);
	SAFE_DELETE(shader);
	SAFE_DELETE(Drawer);
}

void Line::Tick()
{

}

void Line::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	
	ID3D11DeviceContext * Context = D3D::Get()->GetDeviceContext();
	
	Context->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Drawer->Draw(2);
	// shader->Draw(0, 0, 2);
}
