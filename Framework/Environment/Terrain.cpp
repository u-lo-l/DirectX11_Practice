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
	this->CreateNormalData();
	this->CreateBuffer();

	
	WorldMatrix = Matrix::Identity;

	ID3D11DeviceContext * DeviceContext = D3D::Get()->GetDeviceContext();
	
	DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

Terrain::~Terrain()
{
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE_ARR(Vertices);
	SAFE_DELETE_ARR(Indices);
	SAFE_DELETE(Drawer);
	SAFE_DELETE(HeightMap);
}

void Terrain::Tick()
{
	const Context * Ctxt = Context::Get();
	
	CHECK(Drawer->AsMatrix("World")->SetMatrix(WorldMatrix) >= 0);
	CHECK(Drawer->AsMatrix("View")->SetMatrix(Ctxt->GetViewMatrix()) >= 0);
	CHECK(Drawer->AsMatrix("Projection")->SetMatrix(Ctxt->GetProjectionMatrix()) >= 0);
	// CHECK(Drawer->AsVector("LightDirection")->SetFloatVector(Context::Get()->GetLightDirection()) >= 0);
}

void Terrain::Render() const
{
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
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
			const UINT Index = Z * Width + X;
			const UINT PixelIndex = (Height - 1 - Z) * Width + X;
			Vertices[Index].Position.X = static_cast<float>(X);
			Vertices[Index].Position.Y = Pixels[PixelIndex].R * 40;
			Vertices[Index].Position.Z = static_cast<float>(Z);
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

void Terrain::CreateNormalData() const
{
	for (UINT i = 0 ; i < IndexCount / 3; i++)
	{
		const UINT Index0 = Indices[i * 3 + 0];
		const UINT Index1 = Indices[i * 3 + 1];
		const UINT Index2 = Indices[i * 3 + 2];

		TerrainVertexType & Vertex0 = Vertices[Index0];
		TerrainVertexType & Vertex1 = Vertices[Index1];
		TerrainVertexType & Vertex2 = Vertices[Index2];

		Vector E1 = (Vertex1.Position - Vertex0.Position);
		Vector E2 = (Vertex2.Position - Vertex0.Position);
		Vector Normal = Vector::Cross(E1, E2);
		Normal.Normalize();
		
		Vertex0.Normal += Normal;
		Vertex1.Normal += Normal;
		Vertex2.Normal += Normal;
	}

	for (UINT I = 0 ; I < VertexCount ; I++)
	{
		Vertices[I].Normal.Normalize();
	}
}

void Terrain::CreateBuffer()
{
	VBuffer = new VertexBuffer(Vertices, VertexCount, sizeof(TerrainVertexType));
	IBuffer = new IndexBuffer(Indices, IndexCount);
}

