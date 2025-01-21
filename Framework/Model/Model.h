#pragma once
#include <unordered_map>
#include "ModelMesh.h"

class ModelBone;
class ModelMesh;
class ModelAnimation;

class Model
{
public:
	using CachedBoneTableType = unordered_map<string, ModelBone *>;
	// 대부분의 모델에서 250개면 충분하다.
	// Shader에서는 동적할당이 안 돼서 정적 크기를 지정해준다.
	static constexpr UINT MaxModelTransforms = 256;
private:
	
	using ThisClass = Model;
public:
	explicit Model( const wstring & ModelFileName );
	~Model();

	void Tick();
	void Render();

public:
	Transform * GetWorldTransform() const { return WorldTransform; };
	__forceinline void SetWorldPosition(const Vector & InPos)
	{
		WorldTransform->SetPosition(InPos);
		bTransformChanged = true;
	}
	__forceinline void SetWorldRotation(const Vector & InEuler)
	{
		WorldTransform->SetRotation(InEuler);
		bTransformChanged = true;
	}
	__forceinline void SetScale(const Vector & InScale)
	{
		WorldTransform->SetScale(InScale);
		bTransformChanged = true;
	}
	
private:
	static Color JsonStringToColor(const Json::String & InJson);

	string ModelName;
	
	map<string, Material*> MaterialsTable;
	
	vector<ModelMesh *> Meshes;
public:
	int Frame;
private:
	vector<ModelAnimation *> Animations;

	bool bTransformChanged = false;
	Transform * WorldTransform;

#pragma region Pass
public:
	void SetPass(int InPass);
private :
	int Pass;
#pragma endregion

#pragma region Bone Data
public:
	const Matrix * GetBoneTransforms() const { return BoneTransforms; }
	UINT GetBoneCount() const { return Bones.size(); }
private:
	ModelBone * RootBone;
	vector<ModelBone *> Bones;
	Matrix BoneTransforms[MaxModelTransforms];
	CachedBoneTableType * CachedBoneTable = nullptr;

	// KeyFrameAnimation을 Texture로 Bake한 것.
	ID3D11Texture2D * ClipTexture = nullptr;
	ID3D11ShaderResourceView * ClipSRV = nullptr;
	
#pragma endregion Bone Data
	
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
};


