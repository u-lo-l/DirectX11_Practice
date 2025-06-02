#pragma once
#include "CBone.h"

class CSkeletal;
class MeshSubset;

//일단 No Instancing
class CSkeletalMesh
{
	using VertexType = ModelVertex;
public:
	// InMeshNames : Adam/Mesh/Adam.mesh
	explicit CSkeletalMesh(const wstring & InModelName);
	~CSkeletalMesh();
	void Tick();
	void Render();

private:
	void ReadTransform(const Json::Value::const_iterator::reference Root);
	void ReadMaterial(const Json::Value::const_iterator::reference Root);
	void ReadMeshAndCreateBoneMap(const Json::Value::const_iterator::reference Root);
	static void ReadShaderName(const Json::Value & Value, Material<VertexType> * OutMatData, bool bUseAnimation = false);
	static void ReadColor(const Json::Value & Value, Material<VertexType> * MatData);
	static void ReadTextures(const Json::Value & Value, Material<VertexType> * MatData);
	void ReadSubMeshes(const BinaryReader * InBinReader);
	void ReadSkeletalData(const BinaryReader* InBinReader, vector<CBone*>& OutBones);
	Transform * Tf;
	map<string, Material<VertexType>*> Materials;
	
	// Skeleton * Skeletal;
	CSkeletal * Skeleton; 
	vector<MeshSubset *> MeshSubsets;

	
	ConstantBuffer * CB_Matrix;
	ConstantBuffer * CB_Light;
};
