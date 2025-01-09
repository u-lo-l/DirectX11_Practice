#pragma once

struct MeshData;

class ModelMesh
{
private:
	using ThisClass = ModelMesh;
	using ThisClassPtr = ThisClass*;
	using VertexType = ModelVertex;
	friend class Model;
	
	static void ReadFile(
		const BinaryReader * InReader,
		vector<ThisClassPtr> & OutMeshes,
		const map<string, Material*> & InMaterialTable
	);
	
	ModelMesh();
	~ModelMesh();

	void Tick() const;
	void Render() const;

	void BindData();
	string Name = "";
	
	Material * MaterialData = nullptr;
	
	int BoneIndex = 0;;
	class ModelBone * Bone = nullptr;
	
	UINT VerticesCount = 0;
	VertexType * Vertices = nullptr;
	
	UINT IndicesCount = 0;
	UINT* Indices = nullptr;
	
	Matrix WorldMatrix = Matrix::Identity;
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;

};
