struct VertexInput
{
    float4 Position : POSITION;
    float4 Color : Color;
    
    uint VertexID : SV_VertexID; // SV_ : 시스템에서 넣어주는 값. Vertex들어오는 순서대로 넣어준다.
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float4 Color : Color;
    uint VertexID : VertexID; // SV_ 없음. 사용자가 넣어주는 값이라는 의미.
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = input.Position;
    output.Color = input.Color;
    output.VertexID = input.VertexID;

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    if (input.VertexID >= 2 && input.VertexID <= 3)
        return float4(1, 1, 1, 1);
    
    return input.Color; // r,g,b,a
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}