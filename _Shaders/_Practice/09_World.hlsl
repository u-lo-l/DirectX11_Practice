cbuffer W : register(b0)
{
    matrix WorldMat;
};

cbuffer V : register(b1)
{
    matrix ViewMat;
};

cbuffer P : register(b2)
{
    matrix ProjectionMat;
};

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
    output.Position = input.Position;

    output.Position = mul(output.Position, WorldMat);
    output.Position = mul(output.Position, ViewMat);
    output.Position = mul(output.Position, ProjectionMat);

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    return float4(1,1,0,1); // r,g,b,a
}