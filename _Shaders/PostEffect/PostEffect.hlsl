struct VS_Input
{
    float4 Position : POSITION;
};

struct VS_Output
{
    float4 Position : SV_POSITION;
    float2 UV : UV;
};
const static int   GaussianBlurCount = 6;
const static float GaussianWeights[13] =
{
    0.0090f, 0.0218f, 0.0448f, 0.0784f, 0.1169f, 0.1486f,
    0.1610f,
    0.1486f, 0.1169f, 0.0784f, 0.0448f, 0.0218f, 0.0090f
};
Texture2D       Texture00           : register(t0);
Texture2D       Texture01           : register(t1);

#define DiffuseMap             Texture00
#define GaussianBlurXTexture   Texture00
#define GaussianBlurYTexture   Texture01
#define OriginTexture          Texture00
#define GaussianBlurTexture    Texture01

SamplerState    LinearSampler       : register(s0);
SamplerState    AnisotropicSampler  : register(s1);

cbuffer CB_PostEffect : register(b0)
{
    float2 PixelSize;
    float  Time;
    float  EffectIndex;
    float  RadialBlurRadius;
    float  RadialBlurAmount; //4%
    float2 RadialBlurScreenCenter;
};

VS_Output VSMain(VS_Input Input)
{
    VS_Output output;
    output.Position = Input.Position;
    output.UV.x = Input.Position.x * 0.5f + 0.5f;
    output.UV.y = -Input.Position.y * 0.5f + 0.5f;

    return output;
}

float4 PS_Diffuse(VS_Output Input);
float4 PS_Inverse(VS_Output Input);
float4 PS_GrayScale(VS_Output Input);
float4 PS_Sharpen(VS_Output Input);
float4 PS_Sepia(VS_Output Input);
float4 PS_Vignette(VS_Output Input);
float4 PS_LensDistortion(VS_Output Input);
float4 PS_Interace(VS_Output Input);
float4 PS_Wiggle(VS_Output Input);
float4 PS_Blur(VS_Output Input);
float4 PS_RadialBlur(VS_Output Input);
float4 PS_GaussianBlur(VS_Output Input);
float4 PS_Bloom(VS_Output Input);

float4 PSMain(VS_Output Input) : SV_TARGET
{
    // return float4(1,1,1,1);
    [flatten] if (0 < EffectIndex && EffectIndex <= 1)
        return PS_Inverse(Input); // 1
    [flatten] if (1 < EffectIndex && EffectIndex <= 2)
        return PS_GrayScale(Input); // 2
    [flatten] if (2 < EffectIndex && EffectIndex <= 3)
        return PS_Sharpen(Input); // 3
    [flatten] if (3 < EffectIndex && EffectIndex <= 4)
        return PS_Sepia(Input); // 4
    [flatten] if (4 < EffectIndex && EffectIndex <= 5)
        return PS_Vignette(Input); // 5
    [flatten] if (5 < EffectIndex && EffectIndex <= 6)
        return PS_LensDistortion(Input); // 6
    [flatten] if (6 < EffectIndex && EffectIndex <= 7)
        return PS_Interace(Input); // 7
    [flatten] if (7 < EffectIndex && EffectIndex <= 8)
        return PS_Wiggle(Input);// 8
    [flatten] if (8 < EffectIndex && EffectIndex <= 9)
        return PS_Blur(Input); // 9
    [flatten] if (9 < EffectIndex && EffectIndex <= 10)
        return PS_RadialBlur(Input); // 10
    [flatten] if (10 < EffectIndex && EffectIndex <= 11)
        return PS_GaussianBlur(Input); // 11
    return PS_Diffuse(Input); // 0
}

/*==========================================================================================*/

float4 PS_Diffuse(VS_Output Input)
{
    return DiffuseMap.Sample(LinearSampler, Input.UV);
}

float4 PS_Inverse(VS_Output Input) 
{
    return float4(1 - DiffuseMap.Sample(LinearSampler, Input.UV).rgb, 1);
}

float4 PS_GrayScale(VS_Output Input) 
{
    float3 RGB = DiffuseMap.Sample(LinearSampler, Input.UV).rgb;
    float Gray = dot(RGB, float3(0.299, 0.587, 0.114));
    return float4(Gray, Gray, Gray, 1.f);
}

float4 PS_Sharpen(VS_Output Input) 
{
    float4 Center = DiffuseMap.Sample(LinearSampler, Input.UV);
    float4 Top    = DiffuseMap.Sample(LinearSampler, Input.UV + float2(0, -PixelSize.y));
    float4 Bottom = DiffuseMap.Sample(LinearSampler, Input.UV + float2(0, +PixelSize.y));
    float4 Left   = DiffuseMap.Sample(LinearSampler, Input.UV + float2(-PixelSize.x, 0));
    float4 Right  = DiffuseMap.Sample(LinearSampler, Input.UV + float2(+PixelSize.x, 0));

    float4 edge = Center * 4 - Top - Bottom - Left - Right;

    return float4(edge.xyz, 1);
}

const static float3x3 ColorToSepiaMatrix = float3x3
(
    0.393, 0.769, 0.189,
    0.349, 0.686, 0.168,
    0.272, 0.534, 0.131
);
float4 PS_Sepia(VS_Output Input)
{
    float4 color = DiffuseMap.Sample(LinearSampler, Input.UV);
    float3 sepiaColor = mul(ColorToSepiaMatrix, color.rgb);  // RGB만 변환
    return float4(sepiaColor, color.a);  // 원래 Alpha 값 유지
}

const static float VignettePower = 4.0f; // 1 - Linear, 1 > None-Linear
const static float2 VignetteScale = float2(1, 1);
float4 PS_Vignette(VS_Output Input)
{
    float4 color = DiffuseMap.Sample(LinearSampler, Input.UV);
    
    float radius = length((Input.UV - 0.5f) * 2.0f / VignetteScale);
    float vignette = pow(abs(radius + 0.00001f), VignettePower);
    
    return saturate(1 - vignette) * color;
}

const static float LensPower = 4;
const static float3 LensDistortion = float3(-0.02f, -0.02f, -0.02f);
float4 PS_LensDistortion(VS_Output Input)
{
    float2 uv = Input.UV * 2.0f - 1.0f;
    
    float2 vpSize = float2(1.0f / PixelSize.x, 1.0f / PixelSize.y);
    float aspect = vpSize.x / vpSize.y;
    float radiusSquared = aspect * aspect + uv.x * uv.x + uv.y * uv.y;
    float radius = sqrt(radiusSquared);

    float3 temp = LensDistortion * pow(abs(radius + 1e-6f), LensPower) + 1;
    
    float2 r = (temp.r * uv + 1) * 0.5f;
    float2 g = (temp.g * uv + 1) * 0.5f;
    float2 b = (temp.b * uv + 1) * 0.5f;
    
    float3 color = 0;
    color.r = DiffuseMap.Sample(LinearSampler, r).r;
    color.g = DiffuseMap.Sample(LinearSampler, g).g;
    color.b = DiffuseMap.Sample(LinearSampler, b).b;

    return float4(color, 1.0f);
}

const static float InteraceStrength = 1.0f;
const static int InteraceValue = 2.0f;
float4 PS_Interace(VS_Output Input) // CRT TV나 오래된 모니터의 스캔라인 효과
{
    float3 RGB = DiffuseMap.Sample(LinearSampler, Input.UV).rgb;
    float height = 1.0f / PixelSize.y;
    
    int value = (int) ((floor(Input.UV.y * height) % InteraceValue) / (InteraceValue * 0.5f));

    [flatten]
    if(value)
    {
        float  GrayLuminance = dot(RGB, float3(0.299, 0.587, 0.114));
        GrayLuminance = min(0.999f, GrayLuminance);
        RGB = lerp(RGB, RGB * GrayLuminance, InteraceStrength);
    }

    return float4(RGB, 1.0f);
}

const static float2 WiggleOffset = float2(10, 10);
const static float2 WiggleAmount = float2(0.01f, 0.01f);
float4 PS_Wiggle(VS_Output Input)
{
    float2 uv = Input.UV;
    
    uv.x += sin(Time + uv.x * WiggleOffset.x) * WiggleAmount.x;
    uv.y += cos(Time + uv.y * WiggleOffset.y) * WiggleAmount.y;
    
    return float4(DiffuseMap.Sample(LinearSampler, uv).rgb, 1.0f);
}

const static uint BlurCount = 8;
float4 PS_Blur(VS_Output Input)
{
    float2 arr[9] =
    {
        float2(-1, -1), float2(+0, -1), float2(+1, -1),
        float2(-1, +0), float2(+0, +0), float2(+1, +0),
        float2(-1, +1), float2(+0, +1), float2(+1, +1),
    };

    float3 color = 0;
    for (uint blur = 0; blur < BlurCount; blur++)
    {
        for (int i = 0; i < 9; i++)
        {
            float x = arr[i].x * (float) blur * PixelSize.x;
            float y = arr[i].y * (float) blur * PixelSize.y;
            
            float2 uv = Input.UV + float2(x, y);
            color += DiffuseMap.Sample(LinearSampler, uv).rgb;
        }
    }

    color /= BlurCount * 9;

    return float4(color, 1.0f);
}

const static float Epsilon = 1e-6f;
float4 PS_RadialBlur(VS_Output Input)
{
    float2 PixelScreenPosition = Input.UV;
    float2 RadiusVector = PixelScreenPosition - RadialBlurScreenCenter;
    float r = length(RadiusVector) + Epsilon;
    RadiusVector /= r;

    r = saturate(2 * r / RadialBlurRadius);

    float2 delta = RadiusVector * r * r * RadialBlurAmount / BlurCount;
    delta = -delta;

    float3 RGB = 0;
    for (uint i = 0; i < BlurCount; i++)
    {
        RGB += DiffuseMap.Sample(AnisotropicSampler, Input.UV).rgb;
        Input.UV += delta;
    }
    RGB /= BlurCount;

    return float4(RGB, 1);
}

float4 PS_GaussianBlur(VS_Output Input) // 1-Pass Blur. Bad
{
    float2 uv = Input.UV;
    float u = PixelSize.x;
    float v = PixelSize.y;
    
    float sum = 0;
    float4 colorX = 0;
    float4 colorY = 0;
    for (int i = -GaussianBlurCount; i <= GaussianBlurCount; i++)
    {
        float2 temp = uv + float2(0.0f, v * (float) i);
        float4 OriginColor = DiffuseMap.Sample(LinearSampler, temp);
        colorY += GaussianWeights[GaussianBlurCount + i] * OriginColor * 0.5f;
        
        temp = uv + float2(u * (float) i, 0.0f);
        OriginColor = DiffuseMap.Sample(LinearSampler, temp);
        colorX += GaussianWeights[GaussianBlurCount + i] * OriginColor * 0.5f;
    }
    return float4(colorX + colorY);
}

/*--------------------------------2PASS Blur--------------------------------------*/

const static float3 BloomThreshold = float3(0.2126, 0.7152, 0.0722);
float4 PS_Bloom_Separate(VS_Output Input) : SV_Target
{
    float4 Color =DiffuseMap.SampleLevel(LinearSampler, Input.UV, 3);
    //  DiffuseMap.Sample(LinearSampler, Input.UV);
    float  Brightness = dot(Color.rgb, BloomThreshold);
    if (Brightness > 0.75)
        return Color;
    else
        return float4(0,0,0,1);
}

float4 PS_GaussianBlur_X(VS_Output Input) : SV_Target
{
    float2 uv = Input.UV;
    float u = PixelSize.x;
    
    float4 colorX = 0;
    for (int i = -GaussianBlurCount; i <= GaussianBlurCount; i++)
    {
        float2 temp = uv + float2(u * (float) i, 0.0f);
        float4 OriginColor = DiffuseMap.Sample(LinearSampler, temp);
        colorX += GaussianWeights[GaussianBlurCount + i] * OriginColor;
    }
    return float4(colorX);
}

float4 PS_GaussianBlur_Y(VS_Output Input) : SV_Target
{
    float2 uv = Input.UV;
    float  v = PixelSize.y;
    
    float4 colorY = 0;
    for (int i = -GaussianBlurCount; i <= GaussianBlurCount; i++)
    {
        float2 temp = uv + float2(0.0f, v * (float) i);
        float4 OriginColor = DiffuseMap.Sample(LinearSampler, temp);
        colorY += GaussianWeights[GaussianBlurCount + i] * OriginColor;
    }

    return float4(colorY);
}

float4 PS_Bloom_Combine(VS_Output Input) : SV_Target
{
    float4 Origin = OriginTexture.Sample(LinearSampler, Input.UV);
    float4 Bloom  = GaussianBlurTexture.Sample(LinearSampler, Input.UV);

    Origin *= (1.0f - saturate(Bloom));
    
    return float4((Origin + Bloom).rgb, 1.0f);
}