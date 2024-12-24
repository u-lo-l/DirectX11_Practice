#include "Pch.h"
#include "IndexBufferDemo.h"

namespace Sdt
{
	const int IndexBufferDemo::VERTEX_COUNT = 4;
	const int IndexBufferDemo::INDEX_COUNT = 6;

	void IndexBufferDemo::Initialize()
	{
		shader = new Shader(L"09_World2.fx");

		// Vertex Buffer
		Vertices = new InnerVertexType[VERTEX_COUNT];

		Vertices[0].Position = Vector(-0.5f, -0.5f, +0.0f);
		Vertices[1].Position = Vector(-0.5f, +0.5f, +0.0f);
		Vertices[2].Position = Vector(+0.5f, -0.5f, +0.0f);
		Vertices[3].Position = Vector(+0.5f, +0.5f, +0.0f);

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
		HRESULT hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
		CHECK(hr);

		// Index Buffer
		Indices = new UINT[INDEX_COUNT]
		{
			0, 1, 2, 2, 1, 3
		};

		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(InnerIndexType) * INDEX_COUNT;
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;


		ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitData.pSysMem = Indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = Device->CreateBuffer(&BufferDesc, &InitData, &IndexBuffer);
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

	void IndexBufferDemo::Destroy()
	{
		SAFE_DELETE_ARR(Indices)
		SAFE_RELEASE(IndexBuffer)
		SAFE_DELETE_ARR(Vertices)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(shader)
	}

	void IndexBufferDemo::Tick()
	{
		
	}

	void IndexBufferDemo::Render()
	{
		shader->AsMatrix("World")->SetMatrix(WorldMat);
		shader->AsMatrix("View")->SetMatrix(ViewMat);
		shader->AsMatrix("Projection")->SetMatrix(ProjectionMat);

		constexpr UINT stride = sizeof(InnerVertexType);
		constexpr UINT offset = 0;

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		D3D::Get()->GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		shader->DrawIndexed(0, 0, 6);

	}
}