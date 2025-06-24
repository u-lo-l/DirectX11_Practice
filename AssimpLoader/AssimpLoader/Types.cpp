#include "Pch.h"
#include "Types.h"

// const aiMaterial * const : 함수 내에서 Material의 불변성을 보장
void MaterialData::CollectTexturePaths( const aiMaterial * const Material, aiTextureType InTextureType )
{
	aiString TextureFile;
	const UINT TextureCount = Material->GetTextureCount(InTextureType);
	ASSERT(TextureCount <= 1, "")

	if (TextureCount == 0)
		return ;
	
	Material->GetTexture(InTextureType, 0, &TextureFile);

	if (InTextureType==aiTextureType_DIFFUSE)
		this->DiffuseFileName = TextureFile.C_Str();
	else if (InTextureType==aiTextureType_SPECULAR)
		this->SpecularFileName = TextureFile.C_Str();
	else if (InTextureType==aiTextureType_NORMALS)
		this->NormalFileName = TextureFile.C_Str();
}
