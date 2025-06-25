#pragma once

class StaticMeshSubset;
class CStaticMesh
{
	using VertexType = VertexStaticMesh;
public:
	explicit CStaticMesh(const wstring & InModelName);
	~CStaticMesh();
	void Tick();
private:
	void ReadTransform(const Json::Value::const_iterator::reference Root) const;
	void ReadMaterial(const Json::Value::const_iterator::reference Root);
	void ReadSubMeshes(const Json::Value::const_iterator::reference Root);
	void ReadSubMeshes(const BinaryReader * InBinReader);

	wstring Name;
	Transform * Tf;
	vector<StaticMeshSubset *> MeshSubsets;
	map<string, Material *> Materials;
};
