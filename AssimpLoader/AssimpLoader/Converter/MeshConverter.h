#pragma once
#include "AConverterBase.h"
#include "AssimpLoader/Types.h"

class MeshConverter : public AConverterBase
{
// protected:
// 	wstring FileName;
// 	Assimp::Importer * Importer;
// 	aiScene * Scene;
public:
	MeshConverter();
	virtual ~MeshConverter() override;
	virtual void ReadAiScene(const wstring& InFileName) override;
private:
	
	void ExportMaterial(const wstring& InFileName, const aiScene* Scene);
	void ExportMesh(const wstring & InFileName, const aiScene* InScene);
	void ExportAsset(const wstring & InFileName);
	
	void ReadMaterial(const aiScene* Scene, vector<MaterialData*>& OutMaterialData);
	void WriteMaterial(const aiScene* InScene, const wstring& InFileName, const vector<MaterialData*>& InMaterials);
	string SaveTextureAsFile(const aiScene* InScene, const string& InSaveFolder, const string& InFilePath);

	void ReadBoneRecursive(const aiNode* InNode, int InIndex, int InParentIndex, vector<BoneData*>& OutBones);
	void ReadMesh(const aiScene* InScene, vector<MeshData*>& OutMeshes);
	void ReadSkinningWeight(const aiScene* InScene, const vector<BoneData*>& InBones, vector<MeshData*>& InOutMeshes);
	void WriteSkin(const wstring& InPath, const vector<BoneData*>& InBones, const vector<MeshData*>&
	                      InMeshes);
	MeshData::VertexType ReadSingleVertex(
		const aiMesh* Mesh,
		UINT VertexIndex,
		const aiMatrix4x4& InMeshTransform_ColMajor
	) const;
};
