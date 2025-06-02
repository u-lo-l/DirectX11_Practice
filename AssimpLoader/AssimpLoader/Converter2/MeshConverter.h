#pragma once
#include "Converter2.h"
#include "AssimpLoader/Types.h"

class MeshConverter : public Converter2
{
// protected:
// 	wstring FileName;
// 	Assimp::Importer * Importer;
// 	aiScene * Scene;
public:
	MeshConverter();
	~MeshConverter() override;
	void ReadAiScene(const wstring& InFileName) override;
private:
	
	static void ExportMaterial(const wstring& InFileName, const aiScene* Scene);
	static void ExportMesh(const wstring & InFileName, const aiScene* InScene);
	static void ExportAsset(const wstring & InFileName);
	
	static void ReadMaterial(const aiScene* Scene, vector<MaterialData*>& OutMaterialData);
	static void WriteMaterial(const aiScene* InScene, const wstring& InFileName, const vector<MaterialData*>& InMaterials);
	static string SaveTextureAsFile(const aiScene* InScene, const string& InSaveFolder, const string& InFileName);

	static void ReadBoneRecursive(const aiNode* InNode, int InIndex, int InParentIndex, vector<BoneData*>& OutBones);
	static void ReadMesh(const aiScene* InScene, vector<MeshData*>& OutMeshes);
	static void ReadSkinningWeight(const aiScene* InScene, const vector<BoneData*>& InBones, vector<MeshData*>& InOutMeshes);
	static void WriteSkin(const wstring& InPath, const vector<BoneData*>& InBones, const vector<MeshData*>&
	                      InMeshes);
	static MeshData::VertexType ReadSingleVertex(
		const aiMesh* Mesh,
		UINT VertexIndex,
		const aiMatrix4x4& InMeshTransform
	);
};
