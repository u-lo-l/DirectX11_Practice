#pragma once

struct MeshData;

class ModelMesh
{
public:
	static constexpr UINT MaxBoneTFCount = 256;
private:
	using ThisClass = ModelMesh;
	using ThisClassPtr = ThisClass*;
	using VertexType = ModelVertex;
	friend class Model;
	
	static void ReadMeshFile(
		const BinaryReader * InReader,
		vector<ThisClassPtr> & OutMeshes,
		const map<string, Material*> & InMaterialTable
	);
	
	ModelMesh();
	~ModelMesh();

	void Tick();
	void Render();
private :
	Shader * CachedShader = nullptr;
	int Pass = 0;
	
	void CreateBuffers();

	string MeshName;

	void SetWorldTransform( const Transform * InTransform ) const;
	Transform * WorldTransform = nullptr;
	
	Material * MaterialData = nullptr;
	
	// Mode::ReadMesh에서 값을 넣어준다.
	int GetBoneIndex() const { return BoneIndex; }
	void SetBoneIndex(int InBoneIndex)
	{
		BoneIndex = InBoneIndex;
		bBoneIndexChanged = true;
	}
	int BoneIndex = 0;;
	bool bBoneIndexChanged = false;
	ModelBone * Bone = nullptr;
	Matrix * Transforms;
	
	UINT VerticesCount = 0;
	VertexType * Vertices = nullptr;
	
	UINT IndicesCount = 0;
	UINT* Indices = nullptr;

	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;
	ConstantDataBinder * CBBinder = nullptr;

	/*
	 * TODO : Mesh가 되게 많은데, Bone정보를 갖는건 이해하겠음.
	 * 그런데 BoneDesc가 모든 Mesh들에 대해서 다 있어야하나?
	 */
	struct BoneDesc
	{
		Matrix BoneTransforms[MaxBoneTFCount];
		UINT BoneIndex;
		float Padding[3];
	};
	BoneDesc BoneData;
	ConstantBuffer * BoneMatrixCBuffer;
	IECB_t * ECB_BoneMatrixBuffer;
	
private :
	// 애니메이션 정보가 담긴 SRV
	ID3D11ShaderResourceView * ClipsSRV = nullptr;
	ID3D11Texture2D * ClipsTexture = nullptr;
	IESRV_t * ClipsSRVVar = nullptr;
	
};
