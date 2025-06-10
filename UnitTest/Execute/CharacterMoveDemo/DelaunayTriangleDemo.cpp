#include "Pch.h"
#include "DelaunayTriangleDemo.h"

void DelaunayTriangleDemo::Initialize()
{
	const Camera * const MainCamera = Context::Get()->GetCamera();
	MainCamera->SetPosition({ 1.0f, 10.0f, 1.0f });
	MainCamera->SetRotation({ Math::PI_OVER2, 0.0f, 0.0f });
	const std::vector<Vector2D> SamplePoints {
	    {-1.8f, -2.1f}, {-0.9f, -1.8f}, {0.2f, -2.2f}, {1.1f, -1.7f}, {1.9f, -1.9f},
		{-2.1f, -0.8f}, {-1.2f, -1.1f}, {0.1f, -0.9f}, {0.8f, -1.2f}, {2.2f, -0.7f},
		{-1.7f,  0.3f}, {-0.8f, -0.2f}, {0.3f,  0.1f}, {1.2f, -0.1f}, {2.1f,  0.2f},
		{-2.2f,  1.3f}, {-1.1f,  0.9f}, {-0.1f,  1.2f}, {0.9f,  0.8f}, {1.8f,  1.1f},
		{-1.9f,  2.2f}, {-0.7f,  1.7f}, {0.2f,  2.1f}, {1.3f,  1.9f}, {2.0f,  1.8f}
	};
	DelaunayTriangulator = new DelaunayTriangulator2D(SamplePoints);
	const vector<Triangle2D> & Triangles = DelaunayTriangulator->GetTriangles();
	for ( const Triangle2D & Triangle : Triangles )
	{
		const array<Vector2D, 3> & TriangleVertices = Triangle.GetVertices();
		for (const Vector2D & V : TriangleVertices )
		{
			Vector Position = {V.X, 0, V.Y};
			Vertices.push_back({Position, Color::Green});
		}	
	}
	VBuffer = new VertexBuffer(
		Vertices.data(),
		Vertices.size(),
		sizeof(VertexType)
	);
	CB_MatrixData = {
		Matrix::Identity,
		Context::Get()->GetViewMatrix(),
		Context::Get()->GetProjectionMatrix(),
	};
	CB_Matrix = new ConstantBuffer(
		ShaderType::VertexShader,
		0,
		&CB_MatrixData,
		"",
		sizeof(WVPDesc),
		false
	);
	Shader = new HlslShader<VertexType>(L"Basic/BasicVertices.hlsl");
	Shader->CreateRasterizerState_WireFrame_NoCull();
}

void DelaunayTriangleDemo::Destroy()
{
	SAFE_DELETE(DelaunayTriangulator);
}

void DelaunayTriangleDemo::Tick()
{
	CB_MatrixData = {
		Matrix::Identity,
		Context::Get()->GetViewMatrix(),
		Context::Get()->GetProjectionMatrix(),
	};
	CB_Matrix->UpdateData(&CB_MatrixData, sizeof(WVPDesc));
}

void DelaunayTriangleDemo::Render()
{
	if (!VBuffer || !Shader)
		return;
	
	VBuffer->BindToGPU();
	CB_Matrix->BindToGPU();
	Shader->Draw(Vertices.size());
}
