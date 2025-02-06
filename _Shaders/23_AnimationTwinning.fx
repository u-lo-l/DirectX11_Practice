#include "00_Context.fx"
#include "00_Material.fx"
#include "00_Animation.fx"

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS_Model(ModelVertexInput input)
{
    VertexOutput output;
    ModelWorldTF = mul(BoneTransforms[BoneIndex], ModelWorldTF);

    output.Position = SetAnimatedBoneToWorldTF(input); // Local_Space(Bone Root Space)
    output.Position = mul(output.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = mul(input.Normal, (float3x3) ModelWorldTF);
    output.Uv = input.Uv;
    
    return output;
}

SamplerState Samp;
float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
	float Light = dot(-LightDirection, normal);
    float3 color = MaterialMaps[0].Sample(Samp, input.Uv).rgb;
    color *= Light;
    return float4(color, 1);
}

RasterizerState FillMode_Wireframe
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Model()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(FillMode_Wireframe);
        SetVertexShader(CompileShader(vs_5_0, VS_Model()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

}