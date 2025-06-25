#include "Pch.h"
#include "Types.h"

// const aiMaterial * const : 함수 내에서 Material의 불변성을 보장

/** 
 * - aiTextureType_NONE = 0,           // 사용되지 않음
 * - aiTextureType_DIFFUSE,            // 기본 색상 텍스처 (Albedo)
 * - aiTextureType_SPECULAR,           // 정반사 색상 (Specular Color)
 * - aiTextureType_AMBIENT,            // 환경광 색상 (거의 안 씀)
 * - aiTextureType_EMISSIVE,           // 자체 발광 텍스처
 * - aiTextureType_HEIGHT,             // 디스플레이스먼트 맵 (Height Map)
 * - aiTextureType_NORMALS,            // 노멀 맵 (Normal Map)
 * - aiTextureType_SHININESS,          // 광택도 맵 (Shininess / Specular Exponent)
 * - aiTextureType_OPACITY,            // 투명도 맵 (Opacity)
 * - aiTextureType_DISPLACEMENT,       // 실제 지오메트리 변형용 Height Map (Height와 비슷)
 * - aiTextureType_LIGHTMAP,           // 라이트맵
 * - aiTextureType_REFLECTION,         // 반사 맵
 * - aiTextureType_BASE_COLOR,         // PBR: Albedo(BaseColor) 텍스처
 * - aiTextureType_NORMAL_CAMERA,      // PBR: 카메라 공간 노멀맵
 * - aiTextureType_EMISSION_COLOR,     // PBR: 발광 컬러 텍스처
 * - aiTextureType_METALNESS,          // PBR: 금속도 텍스처
 * - aiTextureType_DIFFUSE_ROUGHNESS,  // PBR: 러프니스 텍스처
 * - aiTextureType_AMBIENT_OCCLUSION,  // PBR: AO 텍스처
 * - aiTextureType_SHEEN,              // PBR: Sheen (섬유광) 텍스처
 * - aiTextureType_CLEARCOAT,          // PBR: 클리어코트 텍스처
 * - aiTextureType_TRANSMISSION,       // PBR: Transmission (굴절) 텍스처
 * - aiTextureType_UNKNOWN             // 정의되지 않은 사용자 텍스처
 */
void MaterialData::CollectTexturePaths( const aiMaterial * const Material, aiTextureType InTextureType )
{
	aiString TextureFile;
	const UINT TextureCount = Material->GetTextureCount(InTextureType);
	ASSERT(TextureCount <= 1, "")

	if (TextureCount == 0)
		return ;
	
	if (AI_FAILURE == Material->GetTexture(InTextureType, 0, &TextureFile))
		return ;

	switch (InTextureType)
	{
	case aiTextureType_DIFFUSE:
		this->DiffuseFileName = TextureFile.C_Str(); break;
		
	// Legacy-Phong
	case aiTextureType_SPECULAR:
		this->SpecularFileName = TextureFile.C_Str(); break;
	case aiTextureType_NORMALS:
		this->NormalFileName = TextureFile.C_Str(); break;
	case aiTextureType_SHININESS:
		this->ShininessFileName = TextureFile.C_Str(); break;

	//PBR
	case aiTextureType_BASE_COLOR:
		this->AlbedoFileName = TextureFile.C_Str(); break;
	case aiTextureType_METALNESS:
		this->MetallicFileName = TextureFile.C_Str(); break;
	case aiTextureType_DIFFUSE_ROUGHNESS:
		this->RoughnessFileName = TextureFile.C_Str(); break;
		
	default:
		break;
	}
}
