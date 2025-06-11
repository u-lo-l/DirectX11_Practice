#pragma once
#include "Systems/IExecutable.h"

class DelaunayTriangleDemo : public IExecutable
{
private:
	using VertexType = VertexColor;
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual	void Tick() override;
	virtual void Render() override;

	DelaunayTriangulator2D DelaunayTriangulator;
	
	HlslShader<VertexType> * Shader;
	vector<VertexType> Vertices;
	vector<UINT> Indices;
	VertexBuffer * VBuffer;
	IndexBuffer * IBuffer;

	WVPDesc CB_MatrixData;
	ConstantBuffer * CB_Matrix;
};
