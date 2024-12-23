#include "Pch.h"
#include "UserInterface.h"

namespace Sdt
{
	const int UserInterface::VERTEX_COUNT = 3;

	void UserInterface::Initialize()
	{
		shader = new Shader(L"06_Rectangle.fx");

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

		ID3D11Device * Device = D3D::Get()->GetDevice();
		const HRESULT hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
		CHECK(hr);
	}

	void UserInterface::Destroy()
	{
		SAFE_DELETE_ARR(Vertices)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(shader)
	}

	void UserInterface::Tick()
	{
		static float y = 0.5f;
		ImGui::SliderFloat("test slider", &y, -1.0f, 1.0f);

		Vertices[1].Position.y = y;

		// UpdateSubresource : GPU메모리에 있는 리소스를 CPU에서 업데이트 하기 위한 메서드
		// 정적 리소스를 효율적으로 갱신하는 데 사용된다.
		D3D::Get()->GetDeviceContext()->UpdateSubresource(
			this->VertexBuffer,
			0,
			nullptr,
			this->Vertices,
			sizeof(InnerVertexType) * VERTEX_COUNT,
			0
		);
	}

	void UserInterface::Render()
	{
		constexpr UINT stride = sizeof(InnerVertexType);
		constexpr UINT offset = 0;

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		shader->Draw(0, 0, VERTEX_COUNT);

	}
}