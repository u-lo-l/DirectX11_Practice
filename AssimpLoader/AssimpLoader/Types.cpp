#include "Pch.h"
#include "Types.h"

void MaterialData::SetTextureFilesFromAiMaterialByTextureType( const aiMaterial * const Material, aiTextureType InTextureType )
{
	vector<string> * TargetList = nullptr;
	switch (InTextureType)
	{
	case aiTextureType_DIFFUSE:  // NOLINT(bugprone-branch-clone)
		TargetList = &DiffuseFiles;
		break;
	case aiTextureType_SPECULAR:
		TargetList = &SpecularFiles;
		break;
	case aiTextureType_NORMALS:
		TargetList = &NormalFiles;
		break;
	default:
		break;
	}
	if (TargetList == nullptr)
	{
		return;
	}
		
	aiString TextureFile;
	const UINT TextureCount = Material->GetTextureCount(InTextureType);
	TargetList->reserve(TextureCount);

	for (UINT TextureIndex = 0; TextureIndex < TextureCount; TextureIndex++)
	{
		Material->GetTexture(InTextureType, TextureIndex, &TextureFile);
		TargetList->emplace_back(TextureFile.C_Str());
	}
}
