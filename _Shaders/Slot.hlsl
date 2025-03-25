#define InstancingSlot 9

#define Const_VS_World b0
#define Const_VS_VieProjection b1
#define Const_VS_BoneMatrix b2
#define Const_VS_BoneIndex b3
#define Const_VS_AnimationBlending b4

#define Const_PS_Material b5

#define Texture_PS_Texture t0 // Diffuse Texture
// t1 for Specular Texture
// t2 for Normal Texture
#define Texture_VS_KeyFrames t3

#define Sampler_PS_Default s0

#define MAX_MODEL_TRANSFORM 256
#define MAX_INSTANCE_COUNT 200

#define MATERIAL_TEXTURE_DIFFUSE 0
#define MATERIAL_TEXTURE_NORMAL 1
#define MATERIAL_TEXTURE_SPECULAR 2
#define MAX_MATERIAL_TEXTURE_COUNT 3