#include "Pch.h"
#include "Line_Circle.h"
#include <cmath>

void Line_Circle::Initialize()
{
	// shader = new Shader(L"04_Line_Circle.fx");
	hlslShader = new HlslShader<VertexType>(L"04_Line_Circle.hlsl");

	printf("Hello Draw Circle\n");

	for (int i = 0 ; i < 360 ; i++)
	{
		constexpr float Radius = 0.75f;
		const float Rad = Math::ToRadians(static_cast<float>(i));
		const float C = std::cos(Rad) * Radius;
		const float S = std::sin(Rad) * Radius;
		printf("%03d | %.8f (%.8f __ %.8f)\n", i, Rad, C, S);

		float temp = static_cast<float>(i) / 4.f;
		Vertices.emplace_back(Vector(C, S, 0.f), Color(temp, 1, 0, 1));
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(VertexType) * Vertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));

	VertexType * temp = new VertexType[Vertices.size()];
	copy(Vertices.begin(), Vertices.end(), temp);
	subResource.pSysMem = temp;

	const HRESULT hr = D3D::Get()->GetDevice()->CreateBuffer(&desc, &subResource, &VertexBuffer);

	SAFE_DELETE_ARR(temp)
	CHECK(hr >= 0);
}

void Line_Circle::Destroy()
{
	SAFE_RELEASE(VertexBuffer)
	SAFE_DELETE(shader)
	SAFE_DELETE(hlslShader)
}

void Line_Circle::Tick()
{

}

void Line_Circle::Render()
{
	constexpr UINT stride = sizeof(VertexType);
	constexpr UINT offset = 0;

	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	//D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// shader->Draw(0, 0, Vertices.size());
	hlslShader->Draw(Vertices.size());
}

