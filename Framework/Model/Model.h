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
	Model();
	~Model();

	void Tick() const;
	void Render() const;

	void ReadMaterial(const wstring & InFileName);
private:
	void ReadShaderName();
	void ReadColor();
	void ReadColorMap();
public:
	void ReadMesh(const wstring & InFileName);
private:
	Color JsonStringToColor(const Json::String & InJson);
private:
	ModelBone * RootBone;
	vector<ModelBone *> Bones;
	vector<ModelMesh *> Meshes;

	map<string, Material*> MaterialsTable;

	Matrix BoneTransforms[MaxModelTransforms];
};
