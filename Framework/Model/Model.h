#pragma once

class ModelBone;
class ModelMesh;

class Model
{
private:
	// 대부분의 모델에서 250개면 충분하다.
	// Shader에서는 동적할당이 안 돼서 정적 크기를 지정해준다.
	static constexpr UINT MaxModelTransforms = 256;
	
	using ThisClass = Model;
public:
	explicit Model( const wstring & ModelFileName );
	~Model();

	void Tick() const;
	void Render() const;

	void ReadFile(const wstring & InFileFullPath);
public:
	Transform * GetWorldTransform() const { return WorldTransform; };
	
	
#pragma region Read Material Data

public:
	void ReadMaterial(const wstring & InFileName);
private:
	void ReadShaderName(const Json::Value & Value, Material * MatData);
	void ReadColor(const Json::Value & Value, Material * MatData);
	void ReadColorMap(const Json::Value & Value, Material * MatData);

#pragma endregion

#pragma region Read Mesh Data

public:
	void ReadMesh(const wstring & InFileName);

#pragma endregion

	
private:
	Color JsonStringToColor(const Json::String & InJson);
private:
	ModelBone * RootBone;
	vector<ModelBone *> Bones;
	vector<ModelMesh *> Meshes;

	map<string, Material*> MaterialsTable;

	Matrix BoneTransforms[MaxModelTransforms];
private:
	Transform * WorldTransform;

#pragma region Pass
public:
	void SetPass(int InPass);
private :
	int Pass;
#pragma endregion

public:
	const Matrix * GetBoneTransforms() const { return BoneTransforms; }
	UINT GetBoneCount() const { return Bones.size(); }
	
};
