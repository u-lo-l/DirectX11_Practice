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
const static float3 PosY = float3(0, 1, 0);
const static float3 NegY = White - PosY;
float4 PSMain(VertexOutput input) : SV_Target
{
    // float Color = Texture.Sample(LinearSampler, input.Uv).g;
    // float3 Green = pow(abs(Color) * 10 , 0.75f)* (Color < 0 ?  NegY : PosY);

    // return float4(Green, 1);

    float Color = Texture.Sample(LinearSampler, input.Uv).g * sqrt(20*20 + 20*20) / 2;
    Color = (Color + 1) / 2;
    float3 Green = float3(Color, Color, Color);

    return float4(Green, 1);
}

#endif