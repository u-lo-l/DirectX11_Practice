#pragma once

struct MeshData;

class ModelMesh
{
private:
	using ThisClass = ModelMesh;
	using ThisClassPtr = ThisClass*;
	using VertexType = ModelVertex;
	friend class Model;
	
	static void ReadFile( const BinaryReader * InReader, vector<ThisClassPtr> & OutMeshes );
	
	ModelMesh();
	~ModelMesh();

	void Tick() const;
	void Render() const;

	void BindData(Shader * InRenderer);
	string Name = "";
	string MaterialName = "";
	
	int BoneIndex = 0;;
	class ModelBone * Bone = nullptr;
	
	UINT VerticesCount = 0;
	VertexType * Vertices = nullptr;
	
	UINT IndicesCount = 0;
	UINT* Indices = nullptr;
	Shader * Renderer = nullptr;
	Matrix WorldMatrix = Matrix::Identity;
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;
};
