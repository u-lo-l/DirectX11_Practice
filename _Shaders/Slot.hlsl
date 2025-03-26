#define InstancingSlot 9

#define Const_VS_World              b0
#define Const_VS_VieProjection      b1
#define Const_VS_BoneMatrix         b2
#define Const_VS_BoneIndex          b3
#define Const_VS_AnimationBlending  b4

#define Const_PS_Material           b5
#define Const_PS_LightDirection     b6

#define Const_GS_World              b9
#define Const_GS_VieProjection      b10

#define Texture_VS_KeyFrames        t0
#define Texture_PS_Billboard        t0
#define Texture_PS_Texture          t1 // Diffuse Texture
//                                  t2    Specular Texture
//                                  t3    Normal Texture

#define Sampler_PS_Linear           s0

#define MAX_MODEL_TRANSFORM 256
#define MAX_INSTANCE_COUNT  200

#define MATERIAL_TEXTURE_DIFFUSE    0
#define MATERIAL_TEXTURE_NORMAL     1
#define MATERIAL_TEXTURE_SPECULAR   2
#define MAX_MATERIAL_TEXTURE_COUNT  3