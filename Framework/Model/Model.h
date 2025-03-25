#pragma once
#include <unordered_map>
#include "SkeletalMesh.h"
#include "Skeleton.h"

class ModelBone;
class ModelMesh;
class ModelAnimation;

class Model
{
public:
	using CachedBoneTableType = unordered_map<string, ModelBone *>;
	constexpr static int MaxModelInstanceCount = 200;
private:
	using ThisClass = Model;

	/*====================================================================================*/

public:
	explicit Model( const wstring & ModelFileName );
	Model( const wstring & ModelFileName, const Vector& Pos, const Quaternion& Rot, const Vector& Scale);
	~Model();

	void Tick();
	void Render() const;

	/*====================================================================================*/

private:
	// TODO : 이거 있을 위치가 여기는 아닌 것 같은데...
	static Color JsonStringToColor(const Json::String & InJson);
	string ModelName;
	map<string, Material*> MaterialsTable;
	vector<ModelMesh *> Meshes;
	Transform * WorldTransform;

#pragma region Instancing
public:
	Transform * AddTransforms();
	const Transform * GetTransforms(UINT Index) const;
private:
	// 모델을 인스턴싱을 통해서 렌더링 할 때는 여러 WorldTransform이 필요하다.
	// 한 모델의 여러 인스턴스들이 각각의 WorldTFMatrix에 매칭된다.
	vector<Transform*> WorldTransforms; // 실제 인스턴스의 Transform
	Matrix WorldTFMatrix[MaxModelInstanceCount]; // Shader(InstanceBuffer)에 넘겨줄 배열

	InstanceBuffer * InstBuffer = nullptr;
#pragma endregion Instancing
	
	/*====================================================================================*/

#pragma region Bone Data
private:
	Skeleton * SkeletonData = nullptr; 
#pragma endregion Bone Data

	/*====================================================================================*/

	// TODO : Animation이 없을 수도 있는데...
#pragma region Animation Data
public:
	UINT GetClipCount() const { return Animations.size(); }
	void SetClipIndex(UINT InInstanceID, int InClipIndex);
	int ClipIndex = 0;
private:
	vector<ModelAnimation *> Animations {};
	ID3D11ShaderResourceView * KeyFrameSRV2DArray = nullptr;

	/*--------------------------------------------*/

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
	void CreateAnimationBuffers();
	void UpdateCurrentFrameData(int InstanceId = 0);
	void UpdateNextFrameData(int InstanceId = 0);
	void UpdateFrameData(FrameDesc & FrameData) const;

	AnimationBlendingDesc BlendingDatas[MaxModelInstanceCount];
	ConstantBuffer * AnimationFrameData_CBuffer;
	
#pragma endregion Animation Data
	
/*====================================================================================*/
	
#pragma region ReadFile
	
public:
	void ReadFile(const wstring & InFileFullPath);

	#pragma region Read Material Data
	public:
		void ReadMaterial(const wstring & InFileName);
	private:
		static void ReadShaderName(const Json::Value & Value, Material * MatData);
		static void ReadColor(const Json::Value & Value, Material * MatData);
		static void ReadColorMap(const Json::Value & Value, Material * MatData);
	#pragma endregion Read Material Data

	#pragma region Read Mesh Data
	public:
		void ReadMeshAndCreateBoneTable(const wstring & InFileName);
	#pragma endregion Read Mesh Data

	#pragma region Read Animation Data
	public :
		void ReadAnimation(const wstring & InFileName);
		void CreateAnimationTextureAndSRV();
	#pragma endregion Read Animation Data
	
#pragma endregion ReadFile

/*====================================================================================*/
};



