#include "Pch.h"
#include "Types.h"

// const aiMaterial * const : 함수 내에서 Material의 불변성을 보장
void MaterialData::SetTextureFilesFromAiMaterialByTextureType( const aiMaterial * const Material, aiTextureType InTextureType )
{
	vector<string> * TargetList = nullptr;

	if (InTextureType==aiTextureType_DIFFUSE)
		TargetList = &this->DiffuseFiles;
	else if (InTextureType==aiTextureType_SPECULAR)
		TargetList = &this->SpecularFiles;
	else if (InTextureType==aiTextureType_NORMALS)
		TargetList = &this->NormalFiles;
	else
		return ;
		
	aiString TextureFile;
	const UINT TextureCount = Material->GetTextureCount(InTextureType);
	TargetList->reserve(TextureCount);

	for (UINT TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
	{
		Material->GetTexture(InTextureType, TextureIndex, &TextureFile);
		TargetList->emplace_back(TextureFile.C_Str());
	}
}
