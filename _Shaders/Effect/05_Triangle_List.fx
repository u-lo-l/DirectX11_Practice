struct VertexInput
{
    float4 Position : POSITION;
    float4 Color : Color;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float4 Color : Color;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = input.Position;
    output.Color = input.Color;

    return output;
}

float4 PS01(VertexOutput input) : SV_Target
{
    return input.Color; // r,g,b,a
}

float4 PS02(VertexOutput input) : SV_Target
{
    return (1.0f - input.Color); // r,g,b,a
}

float4 PS03(VertexOutput input) : SV_Target
{
    return (input.Color.zyxw); // r,g,b,a
}


technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS01()));
    }
    // pass를 둘로 나눈 이유?
    // P0은 정점의 기본 색상을 그대로 출력하며, 
    // P1은 색상을 반전하여 출력하기 때문에 별도의 pass로 나누어 처리
    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS02()));
    }
    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS03()));
    }
}