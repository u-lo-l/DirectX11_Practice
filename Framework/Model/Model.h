#pragma once

class ModelBone;
class ModelMesh;

class Model
{
private:
	using ThisClass = Model;
public:
	Model();
	~Model();

	void Tick() const;
	void Render() const;

	void ReadMesh(const wstring & InFileName);
	void BindData(Shader * InShader);
private:
	ModelBone * RootBone;
	vector<ModelBone *> Bones;
	vector<ModelMesh *> Meshes;
};
