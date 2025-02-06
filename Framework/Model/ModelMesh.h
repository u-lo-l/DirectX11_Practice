#pragma once

class ModelAnimation;
class ModelBone;
struct MeshData;

class ModelMesh
{
public:
	using ThisClass = ModelMesh;
	using ThisClassPtr = ThisClass*;
	friend class Model;
protected:
	using VertexType = ModelVertex;
	
	static void ReadMeshFile(
		const BinaryReader * InReader,
		vector<ThisClassPtr> & OutMeshes,
		const map<string, Material*> & InMaterialTable,
		bool bIsSkeletal = false
	);

#ifdef DO_DEBUG
	explicit ModelMesh(const string & MetaData = "ModelMesh");
	string MetaData;
#else
	ModelMesh();
#endif
	virtual ~ModelMesh() = 0;
	// virtual void Tick(const ModelAnimation * CurrentAnimation = nullptr);
	virtual void Tick(UINT InInstanceSize, const vector<ModelAnimation *> & InAnimations);
	virtual void Render(bool bInstancing = false);
	virtual void CreateBuffers();
	virtual void CreateAnimationBuffers();
	
	void SetWorldTransform( const Transform * InTransform ) const;
	
//TODO : private으로 바꾸기
protected :
	Shader * CachedShader = nullptr;
	int Pass = 0;

	string MeshName;

	Transform * ref_ModelWorldTransform = nullptr;

	Material * MaterialData = nullptr;

	UINT VerticesCount = 0;
	VertexType * Vertices = nullptr;
	
	UINT IndicesCount = 0;
	UINT* Indices = nullptr;

	VertexBuffer * VBuffer = nullptr;
	IndexBuffer * IBuffer = nullptr;
	//View-Projection Matrix정보임.
	ConstantDataBinder * GlobalMatrixCBBinder = nullptr;

	
#pragma region Animation
protected:
	struct FrameDesc
	{
		int		Clip = -1;
		float	CurrentTime = 0;
		int		CurrentFrame;
		int		NextFrame;
	};
	
	struct AnimationBlendingDesc
	{
		float BlendingDuration = 0.1f;
		float ElapsedBlendTime = 0.0f;
		float Padding[2];

		FrameDesc Current;
		FrameDesc Next;
	};
	// void UpdateCurrentFrameData_NonInstancing(const ModelAnimation * InAnimation);
	// void UpdateNextFrameData_NonInstancing(const ModelAnimation * InAnimation);

	void UpdateCurrentFrameData_Instancing(const ModelAnimation * InAnimation, int InstanceId);
	void UpdateNextFrameData_Instancing(const ModelAnimation * InAnimation, int InstanceId);
	
private:
	static void UpdateFrameData(const ModelAnimation * InAnimation, FrameDesc & Frame);
protected:
	// AnimationBlendingDesc BlendingData;
	AnimationBlendingDesc BlendingDatas[25];
protected:
	ConstantBuffer * FrameCBuffer;
	IECB_t * ECB_FrameBuffer;
	
protected :
	// 애니메이션 정보가 담긴 SRV
	ID3D11ShaderResourceView * ClipsSRV = nullptr;
	ID3D11Texture2D * ClipsTexture = nullptr;
	IESRV_t * ClipsSRVVar = nullptr;
	
#pragma endregion Animation
};
