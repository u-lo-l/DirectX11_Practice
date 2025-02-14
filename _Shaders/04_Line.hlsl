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
    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    return float4(0, 1, 0, 1); // r,g,b,a
}