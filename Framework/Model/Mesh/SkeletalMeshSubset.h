#pragma once

//일단 No Instancing
// MeshSubset For SkeletalMesh
class SkeletalMeshSubset final : public ARenderable
{
private:
	using VertexType = VertexSkeletalMesh;
public:
	struct MeshSubsetDesc
	{
		string Name;
		vector<VertexType> Vertices;
		vector<UINT> Indices;
		const Material * Material;
	};
	explicit SkeletalMeshSubset(const MeshSubsetDesc & Desc);
	virtual ~SkeletalMeshSubset() override;
	void SetSkeletal(const CSkeletal* Skeletal);

private:
	virtual void BindResources() const override;
	const CSkeletal * Skeleton;
	vector<VertexType> Vertices;
	vector<UINT> Indices;
};
