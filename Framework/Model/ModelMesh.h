#pragma once

struct MeshData;

class ModelMesh
{
private:
	static constexpr UINT MaxModelTransforms = 256;
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

	void Tick();
	void Render() const;

	void CreateBuffers();
	string Name = "";
	
	Material * MaterialData = nullptr;

	// Mode::ReadMesh에서 값을 넣어준다.
	int BoneIndex = 0;;
	class ModelBone * Bone = nullptr;
	Matrix * Transforms;
	
	UINT VerticesCount = 0;
	VertexType * Vertices = nullptr;
	
	UINT IndicesCount = 0;
	UINT* Indices = nullptr;
	
	Matrix WorldMatrix = Matrix::Identity;
	
	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;
	ConstantDataBinder * CBBinder = nullptr;

	/*
	 * 의문 : Mesh가 되게 많은데, Bone정보를 갖는건 이해하겠음.
	 * 그런데 BoneDesc가 모든 Mesh들에 대해서 다 있어야하나?
	 */
	struct BoneDesc
	{
		Matrix Transforms[MaxModelTransforms];
		UINT BoneIndex;
		float Padding[3];
	};
	BoneDesc BoneData;
	ConstantBuffer * BoneMatrixCBuffer;
	IECB_t * ECB_BoneMatrixBuffer;
};
