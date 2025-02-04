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
private:
	using ThisClass = Model;

/*====================================================================================*/

public:
	explicit Model( const wstring & ModelFileName );
	Model( const wstring & ModelFileName, const Vector& Pos, const Quaternion& Rot, const Vector& Scale);
	~Model();

	void Tick();
	void Render();

#pragma region GettersSetters
public:
	Transform * GetWorldTransform() const { return WorldTransform; }
	const string & GetModelName() const { return ModelName; }
	__forceinline void SetWorldPosition(const Vector & InPos) const	{ WorldTransform->SetPosition(InPos); }
	__forceinline void SetWorldRotation(const Vector & InEuler) const { WorldTransform->SetRotation(InEuler); }
	__forceinline void SetScale(const Vector & InScale) const {	WorldTransform->SetScale(InScale); }
#pragma endregion GettersSetters
	
/*====================================================================================*/

private:
	// TODO : 이거 있을 위치가 여기는 아닌 것 같은데...
	static Color JsonStringToColor(const Json::String & InJson);

	string ModelName;
	
	map<string, Material*> MaterialsTable;
	
	vector<ModelMesh *> Meshes;

	Transform * WorldTransform;
	
/*====================================================================================*/

#pragma region Pass
public:
	void SetPass(int InPass);
private :
	int Pass;
#pragma endregion

/*====================================================================================*/

//TODO : Skeleton정보도 없을 수도 있는데.... BoneTransforms 너무 과한데
#pragma region Bone Data
private:
	Skeleton * SkeletonData;
#pragma endregion Bone Data

/*====================================================================================*/

// TODO : Animation이 없을 수도 있는데...
#pragma region Animation Data
public:
	UINT GetClipIndex() const { return ClipIndex; }
	UINT GetClipCount() const { return Animations.size(); }

	void SetClipIndex(UINT InClipIndex);
	void SetAnimationTime(float InAnimationTime);
	void SetAnimationSpeed(float InAnimationSpeed);
	
private:
	UINT ClipIndex = 0; // 몇 번째 애니메이션의
	UINT Frame; // 몇 frame의 동작인지
	vector<ModelAnimation *> Animations;

	// KeyFrameAnimation을 Texture로 Bake한 것.
	ID3D11Texture2D * ClipTexture = nullptr;
	ID3D11ShaderResourceView * ClipSRV = nullptr;
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
		void ReadMesh(const wstring & InFileName);
	#pragma endregion Read Mesh Data

	#pragma region Read Animation Data
	public :
		void ReadAnimation(const wstring & InFileName);
		void CreateAnimationTexture();
	#pragma endregion Read Animation Data
	
#pragma endregion ReadFile

/*====================================================================================*/

};


