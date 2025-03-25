cbuffer WorldViewProjection : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
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
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    return float4(1, 1, 1, 1);
}