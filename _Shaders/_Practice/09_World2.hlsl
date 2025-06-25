cbuffer VP : register(b0)
{
    matrix ViewMat;
    matrix ProjectionMat;
};

cbuffer WI : register(b1)
{
    matrix WorldMat;
    float Index;
    float3 padding;
}

struct VertexInput
{
    float4 Position : POSITION;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, WorldMat);
    output.Position = mul(output.Position, ViewMat);
    output.Position = mul(output.Position, ProjectionMat);

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    if (Index == 0)
        return float4(1, 0, 0, 1);
    else if (Index == 1)
        return float4(0, 1, 0, 1);
    else if (Index == 2)
        return float4(0, 0, 1, 1); // r,g,b,a
    return float4(0, 0, 0, 1);
}