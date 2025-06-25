#pragma once

class StaticMeshSubset final : public ARenderable
{
private:
	using VertexType = VertexStaticMesh;
public:
	struct MeshSubsetDesc
	{
		string Name;
		vector<VertexType> Vertices;
		vector<UINT> Indices;
		const Material * Material;
	};
	explicit StaticMeshSubset(const MeshSubsetDesc & Desc);
	virtual ~StaticMeshSubset() override;

private:
	virtual void BindResources() const override;
	vector<VertexType> Vertices;
	vector<UINT> Indices;
	array<Matrix, 1> WorldTransforms;
};
