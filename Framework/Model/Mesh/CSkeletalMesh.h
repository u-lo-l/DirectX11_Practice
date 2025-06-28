#pragma once
#include "CBone.h"

class CSkeletal;
class SkeletalMeshSubset;

//일단 No Instancing
class CSkeletalMesh 
{
	using MaterialType = MeshMaterial;
	using VertexType = VertexSkeletalMesh;
public:
	// InMeshNames : Adam/Mesh/Adam.mesh
	explicit CSkeletalMesh(const wstring & InModelName);
	~CSkeletalMesh();
	void Tick();
	CSkeletal * GetSkeletal() const;
	Transform * GetTransform() const;
private:
	void ReadTransform(const Json::Value::const_iterator::reference Root) const;
	void ReadMaterial(const Json::Value::const_iterator::reference Root);
	void ReadSubMeshesAndBones(const Json::Value::const_iterator::reference Root);
	void ReadSubMeshes(const BinaryReader * InBinReader);
	void ReadSkeletalData(const BinaryReader* InBinReader, vector<CBone*>& OutBones);

	wstring Name;
	Transform * Tf;
	CSkeletal * Skeleton; 
	vector<SkeletalMeshSubset *> MeshSubsets;
	map<string, Material *> Materials;
};
