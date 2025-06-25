#ifndef __TEXTURE_FUNC_HLSLI__
#define __TEXTURE_FUNC_HLSLI__

static const uint bUseNormalTexture = 1 << 0;
static const uint bUseDiffuseTexture = 1 << 1;
static const uint bUseSpecularTexture = 1 << 2;
static const uint bUseRoughnessTexture = 1 << 3;
static const uint bUseMetallicTexture = 1 << 4;

bool HasNormalTexture(uint Flag) { return (Flag & bUseNormalTexture) != 0; }
bool HasDiffuseTexture(uint Flag) { return (Flag & bUseDiffuseTexture) != 0; }
bool HasSpecularTexture(uint Flag) { return (Flag & bUseSpecularTexture) != 0; }
bool HasRoughnessTexture(uint Flag) { return (Flag & bUseRoughnessTexture) != 0; }
bool HasMetallicTexture(uint Flag) { return (Flag & bUseMetallicTexture) != 0; }

#endif