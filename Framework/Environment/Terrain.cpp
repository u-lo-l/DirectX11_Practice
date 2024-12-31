#include "framework.h"
#include "Terrain.h"

Terrain::Terrain(const wstring& InShaderFileName, const wstring& InHeightMapFileName)
{
	Drawer = new Shader(InShaderFileName);
	HeightMap = new Texture(InHeightMapFileName);
	Width = HeightMap->GetWidth();
	Height = HeightMap->GetHeight();
	
	this->CreateVertexData();
	this->CreateIndexData();
	this->CreateBuffer();

	WorldMatrix = Matrix::Identity;
}

Terrain::~Terrain()
{
	SAFE_DELETE_ARR(Vertices);
	SAFE_DELETE(Drawer);
	SAFE_DELETE(HeightMap);
}

void Terrain::Tick()
{
	const Context * Ctxt = Context::Get();
	
	CHECK(Drawer->AsMatrix("World")->SetMatrix(static_cast<const float *>(WorldMatrix)) >= 0);
	CHECK(Drawer->AsMatrix("View")->SetMatrix(static_cast<const float *>(Ctxt->GetViewMatrix())) >= 0);
	CHECK(Drawer->AsMatrix("Projection")->SetMatrix(static_cast<const float *>(Ctxt->GetProjectionMatrix())) >= 0);
}

void Terrain::Render()
{
	ID3D11DeviceContext * DeviceContext = D3D::Get()->GetDeviceContext();
	UINT Stride = sizeof(TerrainVertexType);
	UINT Offset = 0;
	
	DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
	DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, Offset);

	Drawer->DrawIndexed(0, Pass, IndexCount);
}

void Terrain::CreateVertexData()
{
	vector<Color> Pixels;
	HeightMap->ReadPixels(Pixels);

	
	VertexCount = Width * Height;
	Vertices = new TerrainVertexType[VertexCount];
	for (UINT Z = 0 ; Z < Height ; Z++)
	{
		for (UINT X = 0 ; X < Width ; X++)
		{
			const UINT PixelIndex = Width * (Height - 1 - Z) + X;
			Vertices[PixelIndex].Position.X = static_cast<float>(X);
			Vertices[PixelIndex].Position.Y = Pixels[PixelIndex].R * 40;
			Vertices[PixelIndex].Position.Z = static_cast<float>(Z);
		}
	}
}

void Terrain::CreateIndexData()
{
	IndexCount = (Width - 1) * (Height - 1) * 6;
	Indices = new UINT[IndexCount];

	UINT Index = 0;
	for (UINT Z = 0 ; Z < Height - 1 ; Z++)
	{
		for (UINT X = 0 ; X < Width  - 1; X++)
		{
			Indices[Index + 0] = Width * Z + X;
			Indices[Index + 1] = Width * (Z + 1) + X;
			Indices[Index + 2] = Width * Z + (X + 1);
			Indices[Index + 3] = Width * Z + (X + 1);
			Indices[Index + 4] = Width * (Z + 1) + X;
			Indices[Index + 5] = Width * (Z + 1) + (X + 1);
			Index += 6;
		}
	}
}

void Terrain::CreateBuffer()
{
	ID3D11Device * Device = D3D::Get()->GetDevice();
	D3D11_BUFFER_DESC BufferDesc;
	D3D11_SUBRESOURCE_DATA SubresourceData;
	
	// VertexBuffer
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = sizeof(TerrainVertexType) * VertexCount;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ZeroMemory(&SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubresourceData.pSysMem = Vertices;
	CHECK(Device->CreateBuffer(&BufferDesc, &SubresourceData, &VertexBuffer) >= 0);

	// IndexBuffer
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = sizeof(UINT) * IndexCount;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ZeroMemory(&SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubresourceData.pSysMem = Indices;
	CHECK(Device->CreateBuffer(&BufferDesc, &SubresourceData, &IndexBuffer) >= 0);
}

