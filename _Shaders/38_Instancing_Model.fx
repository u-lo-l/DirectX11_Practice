// DX ver 11. HLSL 5.0

#include "00_Context.fx"
#include "00_Material.fx"
#include "00_Animation.fx"

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS_Model_Instancing(ModelInstanceVertexInput input)
{    
    VertexOutput output;
    
    ModelWorldTF = mul(BoneTransforms[BoneIndex], input.Transform);

    output.Position = mul(input.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
           
    output.Normal = mul(input.Normal, (float3x3) ModelWorldTF);
    
    output.Uv = input.Uv;
    
    return output;
}

/*
 * Texture의 샘플링 방식을 정의하는 상태블럭
 * 필터링 방식과 UV래핑 방식을 설정한다.
 * Filter : 텍스쳐 필터링(픽셀 단위로 텍스쳐를 어떻게 가져울 지) 방식
 *  - Point : 필셀 그대로 샘플링됨
 *  - LINEAR : 두 개의 Texel을 Lerp
 *  - MIN_MAG_MIP_LINEAR : 축소_확대_mipmap 에 대해 Lerp
 *  - ANISOTROPIC : 각도에 따라 최적화된 고품질 필터링링(비등방성성)
 * AddressU / AddressV : UV좌표가 1을 넘어갈 때 동작 옵션
 *  - Wrap  
 *  - Clamp
 *  - Mirror
 *  - Border. Border의 경우 BorderColor = float(0,0,1,1) 이런식으로 지정 가능능
 */
SamplerState Samp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
	float Light = dot(LightDirection, normal);
    float3 color = MaterialMaps[0].Sample(Samp, input.Uv).rgb;
    color *= Light;
    return float4(color, 1);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Model_Instancing()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}