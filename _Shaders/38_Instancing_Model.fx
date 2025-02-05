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

SamplerState Samp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


// Texture2D Texture;
float4 PS(VertexOutput input) : SV_Target
{
    // float NdotL = dot((input.Normal), -float3(1,0,0));

    // float3 color = Texture.Sample(Samp, input.Uv).rgb * NdotL;
    // return float4(color.rgb, 1);

    float3 normal = normalize(input.Normal);
	float Light = dot(-LightDirection, normal);
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