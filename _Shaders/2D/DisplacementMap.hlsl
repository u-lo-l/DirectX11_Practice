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
const static float3 PosX = float3(1, 0, 0);
const static float3 NegX = White - PosX;
const static float3 PosY = float3(0, 1, 0);
const static float3 NegY = White - PosY;
const static float3 PosZ = float3(0, 0, 1);
const static float3 NegZ = White - PosZ;
float4 PSMain(VertexOutput input) : SV_Target
{
    // float3 Color = Texture.Sample(LinearSampler, input.Uv).rgb;
    // float3 Red =   pow(abs(Color.r) * 10 , 0.75f)* (Color.r < 0 ?  NegX : PosX);
    // float3 Green = pow(abs(Color.g) * 10 , 0.75f)* (Color.g < 0 ?  NegY : PosY);
    // float3 Blue =  pow(abs(Color.b) * 10 , 0.75f)* (Color.b < 0 ?  NegZ : PosZ);
    // return float4(Red + Green + Blue, 1);

    float3 Color = Texture.Sample(LinearSampler, input.Uv).rgb * sqrt(20*20 + 20*20) / 2;
    Color = (Color + 1) / 2;
    return float4(Color, 1);
}

#endif