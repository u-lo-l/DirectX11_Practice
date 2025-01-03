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

void Terrain::GetPositionY(Vector & InPosition ) const
{
	InPosition.Y = Math::FloatMinValue; 
	const UINT X = static_cast<UINT>(InPosition.X);
	const UINT Z = static_cast<UINT>(InPosition.Z);
	if (X < 0 || Z < 0 || X >= Width || Z >= Height)
		return;
	
	UINT Index[4];
	Index[0] = Width * Z + X;
	Index[1] = Width * (Z + 1) + X;
	Index[2] = Width * Z + X + 1;
	Index[3] = Width * (Z + 1) + X + 1;

	Vector V[4];
	for (UINT i = 0; i < 4; i++)
		V[i] = Vertices[Index[i]].Position;
	Vector Start = {InPosition.X, 55 + 10.f, InPosition.Z};
	Vector Direction = {0, -1, 0};
	
	//
	// float SlopX = InPosition.X - V[0].X; 
	// float SlopZ = InPosition.Z - V[0].Z;
	// Vector Result;
	// if (SlopX + SlopZ <= 1.f)
	// 	Result = V[0] + (V[2] - V[0]) * SlopX + (V[1] - V[0]) * SlopZ;
	// else
	// {
	// 	SlopX = 1 - SlopX;
	// 	SlopZ = 1 - SlopZ;
	// 	Result = V[3] + (V[1] - V[3]) * SlopX + (V[2] - V[3]) * SlopZ;
	// }
	//
	// return Result.Y;

	float SlopX, SlopZ, Distance;
	Vector Result(-1, Math::FloatMinValue, -1);
	if (D3DXIntersectTri(V[0], V[1], V[2], Start, Direction, &SlopX, &SlopZ, &Distance) == TRUE)
		Result = V[0] + (V[1] - V[0]) * SlopX + (V[2] - V[0]) * SlopZ;
	if (D3DXIntersectTri(V[3], V[1], V[2], Start, Direction, &SlopX, &SlopZ, &Distance) == TRUE)
		Result = V[3] + (V[1] - V[3]) * SlopX + (V[2] - V[3]) * SlopZ;
	InPosition.Y = Result.Y;
	Gui::Get()->RenderText(5, 60, 1, 0, 0, String::ToString(Result.ToString()));	
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

