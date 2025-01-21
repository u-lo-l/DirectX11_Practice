#pragma once

class ModelBone;
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

#ifdef DO_DEBUG
	explicit ModelMesh(const string & MetaData = "ModelMesh");
	string MetaData;
#else
	ModelMesh();
#endif
	~ModelMesh();
	void Tick();
	void Render(int InFrame = 0);
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
		BoneData.BoneIndex = InBoneIndex;
		bBoneIndexChanged = true;
	}
	void SetBoneTransforms(Matrix * const Transforms)
	{
		this->Transforms = Transforms;
		memcpy(BoneData.BoneTransforms, Transforms, sizeof(Matrix) * MaxBoneTFCount);
	}
	int BoneIndex = 0;
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
	 * GPU에 넘겨줘야하는 정보라 일단 다 가지고 있어야 하긴 함.
	 * 최적화 할 수 있는 방법이 있을거임.
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
	
private:
	struct FrameDesc
	{
		int Frame;
		float Padding[3];
	};
	FrameDesc FrameData;
	ConstantBuffer * FrameCBuffer;
	IECB_t * ECB_FrameBuffer;
	
private :
	// 애니메이션 정보가 담긴 SRV
	ID3D11ShaderResourceView * ClipsSRV = nullptr;
	ID3D11Texture2D * ClipsTexture = nullptr;
	IESRV_t * ClipsSRVVar = nullptr;
	
};
