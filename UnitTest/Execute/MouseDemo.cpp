#include "Pch.h"
#include "MouseDemo.h"

namespace Sdt
{
	UINT MouseDemo::INDEX_PER_GRID = 6;

	void MouseDemo::Initialize()
	{
		shader = new Shader(L"11_Grid.fx");

		// Vertex Buffer
		VertexCount = (Width + 1) * (Height + 1);
		Vertices = new InnerVertexType[VertexCount];

		for (UINT y = 0; y <= Height; y++)
		{
			for (UINT x = 0; x <= Width; x++)
			{
				const UINT Index = (Width + 1) * y + x;
				Vertices[Index].Position.x = static_cast<float>(x);
				Vertices[Index].Position.y = 0.f;
				Vertices[Index].Position.z = static_cast<float>(y);
			}
		}

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
		HRESULT hr = Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
		CHECK(hr >= 0);
		// Vertex Buffer End

		// Index Buffer
		IndexCount = Width * Height * INDEX_PER_GRID;
		Indices = new UINT[IndexCount];

		for (UINT y = 0, Index = 0; y < Height; y++)
		{
			for (UINT x = 0; x < Width; x++, Index += INDEX_PER_GRID)
			{
				Indices[Index + 0] = (Width + 1) * y + x;
				Indices[Index + 1] = (Width + 1) * (y + 1) + x;
				Indices[Index + 2] = (Width + 1) * y + (x + 1);

				Indices[Index + 3] = (Width + 1) * y + (x + 1);
				Indices[Index + 4] = (Width + 1) * (y + 1) + x;
				Indices[Index + 5] = (Width + 1) * (y + 1) + (x + 1);
			}
		}

		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(InnerIndexType) * IndexCount;
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;


		ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
		InitData.pSysMem = Indices;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = Device->CreateBuffer(&BufferDesc, &InitData, &IndexBuffer);
		CHECK(hr >= 0);
		// Index Buffer End

		SAFE_DELETE_ARR(Indices)
		SAFE_DELETE_ARR(Vertices)

		D3DXMatrixIdentity(&WorldMat);

		CamPosition = { 0, 0, -10 };
		const Vector CameraLocation{ 0, 0, -100 };
		const Vector CameraForward{ 0, 0, 1 };
		const Vector CameraUp{ 0, 1, 0 };
		const Vector CameraAt = CamPosition + CameraLocation + CameraForward;
		D3DXMatrixLookAtLH(&ViewMat, &CameraLocation, &CameraAt, &CameraUp);

		const float Aspect = D3D::GetDesc().Width / D3D::GetDesc().Height;
		D3DXMatrixPerspectiveFovLH(&ProjectionMat, Math::PI * 0.25f, Aspect, 0.1f, 1000);
	}

	void MouseDemo::Destroy()
	{
		SAFE_RELEASE(IndexBuffer)
		SAFE_RELEASE(VertexBuffer)
		SAFE_DELETE(shader)
	}

	void MouseDemo::Tick()
	{
		const bool leftPress = Mouse::Get()->IsPress(0);
		const bool rightPress = Mouse::Get()->IsPress(1);
		const bool middlePress = Mouse::Get()->IsPress(2);

		ImGui::LabelText("Left Press", "%d", leftPress ? 1 : 0);
		ImGui::LabelText("Right Press", "%d", rightPress ? 1 : 0);
		ImGui::LabelText("Middle Press", "%d", middlePress ? 1 : 0);

		Vector Position = Mouse::Get()->GetPosition();
		Vector MoveDelta = Mouse::Get()->GetMoveDelta();

		ImGui::LabelText("Mouse", "Position  : <%.2f, %.2f>", Position.x, Position.y);
		ImGui::LabelText("Mouse", "MoveDelta : <%.2f, %.2f, %.2f>", MoveDelta.x, MoveDelta.y, MoveDelta.z);

		static Vector Total = { 0,0,0 };
		Total += MoveDelta;
		ImGui::LabelText("Mouse", "  Total   : <%.2f, %.2f, %.2f>", Total.x, Total.y, Total.z);


		ImGui::SliderInt("Pass", reinterpret_cast<int*>(&Pass), 0, 1);

		ImGui::SliderFloat("X", &CamPosition.x, -256, 256);
		ImGui::SliderFloat("Y", &CamPosition.y, -256, 256);
		ImGui::SliderFloat("Z", &CamPosition.z, -100, 100);
		const Vector CameraLocation = CamPosition + Vector{ 0, 0, -10 };
		const Vector CameraForward{ 0, 0, 1 };
		const Vector CameraUp{ 0, 1, 0 };
		const Vector CameraAt = CameraLocation + CameraForward;
		D3DXMatrixLookAtLH(&ViewMat, &CameraLocation, &CameraAt, &CameraUp);
	}

	void MouseDemo::Render()
	{
		CHECK(shader->AsMatrix("World")->SetMatrix(WorldMat) >= 0);
		CHECK(shader->AsMatrix("View")->SetMatrix(ViewMat) >= 0);
		CHECK(shader->AsMatrix("Projection")->SetMatrix(ProjectionMat) >= 0);

		constexpr UINT stride = sizeof(InnerVertexType);
		constexpr UINT offset = 0;

		D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		D3D::Get()->GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		shader->DrawIndexed(0, Pass, IndexCount);
	}
}