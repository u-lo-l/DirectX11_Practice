#pragma once

//일단 No Instancing
class MeshSubset
{
private:
	using VertexType = ModelVertex;
public:
	struct MeshSubsetDesc
	{
		string Name;
		vector<VertexType> * pVertices;
		vector<UINT> * pIndices;
		Material<VertexType> * pMaterialData;
	};
	explicit MeshSubset(const MeshSubsetDesc & Desc);
	~MeshSubset();
	void Render() const;
	void Tick();
	const string & GetMeshName() { return MeshName; }
	void SetMeshName(const string & InMeshName) { MeshName = InMeshName; }
	void SetMaterialData(Material<VertexType> * InMaterialData) { MaterialData = InMaterialData; }
	void SetTransform(const Matrix & InMatrix);

private:
	string MeshName;
	Material<VertexType> * MaterialData = nullptr;
    vector<VertexType> Vertices;
    vector<UINT> Indices;
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;
	
	Transform * Tf; // Local Transform in Model Root Space
	Matrix LocalTransform;
	ConstantBuffer * CB_LocalTransform;
};
