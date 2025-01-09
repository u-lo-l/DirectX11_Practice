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
};
