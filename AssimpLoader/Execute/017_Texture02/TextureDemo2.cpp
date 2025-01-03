#include "Pch.h"
#include "TextureDemo2.h"


namespace Sdt
{
	void TextureDemo2::Initialize()
	{
		shader = new Shader(L"13_Texture02.fx");
		MainCamera = new Camera();
		
		// Vertex Buffer
		Vertices = new InnerVertexType[VertexCount];
		Vertices[0].Position = {-0.5f, -0.5f, +0.0f};
		Vertices[1].Position = {-0.5f, +0.5f, +0.0f};
		Vertices[2].Position = {+0.5f, -0.5f, +0.0f};
		Vertices[3].Position = {+0.5f, +0.5f, +0.0f};

		Vertices[0].UV = {0, 1};
		Vertices[1].UV = {0, 0};
		Vertices[2].UV = {1, 1};
		Vertices[3].UV = {1, 0};

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(InnerVertexType) * VertexCount;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = 0;
		BufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitData.pSysMem = Vertices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		ID3D11Device* Device = D3D::Get()->GetDevice();
		HRESULT Hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
		CHECK(Hr >= 0);
		// Vertex Buffer End

		// Index Buffer
		Indices = new UINT[IndexCount];

		Indices = new UINT[IndexCount]
		{
			0, 1, 2, 2, 1, 3
		};

		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(InnerIndexType) * IndexCount;
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;


		ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitData.pSysMem = Indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		Hr = Device->CreateBuffer(&BufferDesc, &InitData, &IndexBuffer);
		CHECK(Hr >= 0);
		// Index Buffer End

		SAFE_DELETE_ARR(Indices)
		SAFE_DELETE_ARR(Vertices)

		WorldMat = Matrix::Identity;

		const float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;

		ProjectionMat = Matrix::CreatePerspectiveFieldOfView(Math::Pi * 0.25f, Aspect, 0.1f, 1000.f);
		MainCamera->SetPosition(0,0,-5);

		Hr = D3DX11CreateShaderResourceViewFromFile(
			D3D::Get()->GetDevice(),
			L"../../_Textures/Box/Box.png",
			nullptr,
			nullptr,
			&Srv,
			nullptr
		);
		CHECK(Hr >= 0);

		
	}

	void TextureDemo2::Destroy()
	{
		SAFE_RELEASE(IndexBuffer)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(shader)
		SAFE_DELETE(MainCamera)
	}

	void TextureDemo2::Tick()
	{
		MainCamera->Tick();

		ImGui::SliderInt("Filter", reinterpret_cast<int *>(&Filter), 0, 1);
	}

	void TextureDemo2::Render()
	{
		CHECK(shader->AsMatrix("World")->SetMatrix(WorldMat.ToDX()) >= 0);
		CHECK(shader->AsMatrix("View")->SetMatrix(MainCamera->GetViewMatrix().ToDX()) >= 0);
		CHECK(shader->AsMatrix("Projection")->SetMatrix(ProjectionMat.ToDX()) >= 0);
		CHECK(shader->AsSRV("Map")->SetResource(Srv) >= 0);
		CHECK(shader->AsScalar("Filter")->SetInt(Filter) >= 0);

		constexpr UINT Stride = sizeof(InnerVertexType);
		constexpr UINT Offset = 0;

		ID3D11DeviceContext * DeviceContext = D3D::Get()->GetDeviceContext();
		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
		DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		shader->DrawIndexed(0, 0, IndexCount);
	}
}
