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

float4 PSMain(VertexOutput input) : SV_Target
{
    float3 Color = (Texture.Sample(LinearSampler, input.Uv).rgb + float3(1,1,1)) * 0.5f;
    Color = pow(Color * 1.5f, 5);
    return float4(Color, 1);
}

#endif