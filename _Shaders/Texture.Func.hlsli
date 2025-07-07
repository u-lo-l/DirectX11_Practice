#ifndef __TEXTURE_FUNC_HLSLI__
#define __TEXTURE_FUNC_HLSLI__


static const uint bUseDiffuseTexture = 1 << 1;
static const uint bUseSpecularTexture = 1 << 2;
static const uint bUseRoughnessTexture = 1 << 3;
static const uint bUseMetallicTexture = 1 << 4;

bool HasDiffuseTexture(uint Flag) { return (Flag & bUseDiffuseTexture) != 0; }
bool HasSpecularTexture(uint Flag) { return (Flag & bUseSpecularTexture) != 0; }
bool HasRoughnessTexture(uint Flag) { return (Flag & bUseRoughnessTexture) != 0; }
bool HasMetallicTexture(uint Flag) { return (Flag & bUseMetallicTexture) != 0; }

float3 TangentSpaceToLocalSpace
(
	float3 TextureSpaceVector,
	float3 LocalRight,
	float3 LocalUp,
	float3 LocalForward
)
{
	float3x3 TBN = float3x3(
		LocalRight,
		LocalUp,
		LocalForward
	);
	return mul(TextureSpaceVector, TBN);
}


float3 TangentSpaceToLocalSpace
(
	float3 TextureSpaceVector,
	in float3x3 TBN
)
{
	return mul(TextureSpaceVector, TBN);
}


#endif