#ifndef __DISPLACEMENTMAP_HLSL__
#define __DISPLACEMENTMAP_HLSL__

cbuffer CB_World : register(b0)
{
    matrix World;
};

cbuffer CB_Render2D : register(b1)
{
    matrix View2D;
    matrix Projection2D;
};

Texture2D<float4> Texture : register (t0);
SamplerState LinearSampler : register(s0);

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View2D);
    output.Position = mul(output.Position, Projection2D);
    output.Uv = input.Uv;
    
    return output;
}

const static float3 White = float3(1, 1, 1);
const static float3 PosZ = float3(0, 0, 1);
const static float3 NegZ = White - PosZ;
float4 PSMain(VertexOutput input) : SV_Target
{
    // float3 Color = Texture.Sample(LinearSampler, input.Uv).rgb;
    // float3 Blue =  pow(abs(Color.b) * 10 , 0.5f)* (Color.b < 0 ?  NegZ : PosZ);
    // return float4(Blue, 1);

    float Color = Texture.Sample(LinearSampler, input.Uv).b * sqrt(20*20 + 20*20) / 2;
    Color = (Color + 1) / 2;
    float3 Blue = float3(Color, Color, Color);

    return float4(Blue, 1);
}

#endif