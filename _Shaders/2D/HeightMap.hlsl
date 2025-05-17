#ifndef __HEIGHT_MAP_HLSL__
#define __HEIGHT_MAP_HLSL__

cbuffer CB_World : register(b0)
{
    matrix World;
};

cbuffer CB_Render2D : register(b1)
{
    matrix View2D;
    matrix Projection2D;
};

Texture2D<float> FoamGrid : register (t0);
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
    float mean = (FoamGrid.Sample(LinearSampler, input.Uv));
    return float4(mean, mean, mean, 1);
}

#endif