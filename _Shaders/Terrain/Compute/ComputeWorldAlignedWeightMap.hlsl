#ifndef __COMPUTE_WEIGHT_MAP_HLSL__
# define __COMPUTE_WEIGHT_MAP_HLSL__

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

Texture2D<float>     TerrainHeightMap : register(t0);
RWTexture2D<float4>  Output : register(u0);

cbuffer CB_WeightMapCreate : register(b0)
{
    uint DimX;
    uint DimZ;
    float HeightScaler;
    uint  TextureCount;
}

float SmoothTransition(float x, float edge0, float edge1)
{
    return smoothstep(edge0, edge1, x);
}

float3 GetNormal(uint2 TexCord);

[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
    const float LowHeight = 0.01f * HeightScaler;
    const float MidHeight = 0.3f * HeightScaler;
    const float HighHeight = 0.5f * HeightScaler;
    float HeightBlendRange = 0.05 * HeightScaler;
    float SlopeBlendRange = 5.f;

    float GentleSlope = 20.f;
    float SteepSlope = 50.f;

    const uint2 TexCord = DTID.xy;
    float RawHeight = TerrainHeightMap[TexCord].r; 
    float ScaledHeight = RawHeight * HeightScaler;

    // 현재 위치의 지형 법선 벡터 및 경사 각도 계산
    float3 Normal = GetNormal(TexCord); 
    // Normal 벡터는 Y-up (0,1,0) 기준으로 계산된다고 가정합니다.
    // dot(Normal, float3(0,1,0))은 법선과 수직 벡터 사이의 코사인 값입니다.
    // clamp는 acos의 입력 범위를 [-1, 1]로 제한하여 NaN을 방지합니다.
    float SlopeRad = acos(clamp(dot(Normal, float3(0, 1, 0)), -1.0f, 1.0f));
    float SlopeDegree = SlopeRad * 57.2957764f; // 라디안을 도로 변환

    // === smoothstep을 사용한 부드러운 가중치 계산 ===
    // 각 레이어(Sand, Dirt, Grass, Rock)의 가중치 기여도를 독립적으로 계산합니다.
    // 채널 매핑: R=Sand, G=Dirt, B=Grass, A=Rock 이라고 가정합니다.

    // 고도 기반 기여도 함수들 (Height가 [min, max] 범위에서 0->1 변화)
    // smoothstep(min, max, x)
    float h_min_low = smoothstep(LowHeight, LowHeight + HeightBlendRange, ScaledHeight);    // LowHeight 이상에서 0->1
    float h_low_mid = smoothstep(LowHeight, LowHeight + HeightBlendRange, ScaledHeight);    // LowHeight 이상에서 0->1
    float h_mid_high = smoothstep(MidHeight, MidHeight + HeightBlendRange, ScaledHeight);  // MidHeight 이상에서 0->1
    float h_high_max = smoothstep(HighHeight, HighHeight + HeightBlendRange, ScaledHeight); // HighHeight 이상에서 0->1

    // 경사 기반 기여도 함수들 (SlopeDegree가 [min, max] 범위에서 0->1 변화)
    // smoothstep(min, max, x)
    float s_gentle_max = smoothstep(SteepSlope + SlopeBlendRange, 0.0f, SlopeDegree); // Slope가 Steep 이하일수록 1, 0에 가까울수록 (완만) 1 (0~Steep+Range) -> 1에서 0으로 부드럽게 (Steep+Range에서 0으로)
    // -> 다른 방식으로 정의:
    float s_gentle = smoothstep(SteepSlope, GentleSlope, SlopeDegree); // Slope가 Steep에서 Gentle로 갈 때 0->1
    float s_steep  = smoothstep(GentleSlope, SteepSlope, SlopeDegree); // Slope가 Gentle에서 Steep로 갈 때 0->1

    // --- 각 레이어의 가중치 기여도 계산 ---
    // Sand (R) : 매우 낮은 고도
    // Height가 LowHeight 이하일수록 1, LowHeight + Range 이상에서 0
    float sandContrib = smoothstep(LowHeight + HeightBlendRange, LowHeight, ScaledHeight); 

    // Dirt (G) : LowHeight와 MidHeight 사이의 고도
    // Height가 LowHeight보다 높고 MidHeight보다 낮을수록 1 (두 범위 모두 0->1 변화 곱하기)
    float dirtContrib = smoothstep(LowHeight, LowHeight + HeightBlendRange, ScaledHeight) * (1.0f - smoothstep(MidHeight - HeightBlendRange, MidHeight, ScaledHeight));

    // Grass (B) : MidHeight와 HighHeight 사이의 고도, 그리고 완만한 경사
    // Height가 MidHeight보다 높고 HighHeight보다 낮으면서 (두 범위 모두 0->1 변화 곱하기) + 완만한 경사일수록
    float grassContrib = smoothstep(MidHeight, MidHeight + HeightBlendRange, ScaledHeight) * (1.0f - smoothstep(HighHeight - HeightBlendRange, HighHeight, ScaledHeight)) * s_gentle;

    // Rock (A) : HighHeight보다 높은 고도 또는 가파른 경사
    // Height가 HighHeight 이상이거나 (HighAbove 기여도) + 가파른 경사일수록 (SteepSlope 기여도) -> 둘 중 큰 값 사용 (max)
    float rockContrib = max(smoothstep(HighHeight - HeightBlendRange, HighHeight, ScaledHeight), s_steep);

    // === 최종 가중치 벡터 ===
    // RGBA 채널에 각 레이어의 기여도 할당
    float4 finalWeight = float4(sandContrib, dirtContrib, grassContrib, rockContrib);

    // === 모든 가중치의 합이 1이 되도록 정규화 ===
    // smoothstep 기여도들의 합이 1이 되지 않으므로, 마지막에 반드시 정규화해야 합니다.
    // 이렇게 하면 각 픽셀에서 가장 기여도가 높은 레이어가 지배적인 색상을 결정하게 됩니다.
    finalWeight = normalize(finalWeight); 

    // 최종 가중치 맵에 쓰기
    Output[TexCord] = finalWeight;
}

float3 GetNormal(uint2 TexCord)
{
    uint2 dUV[4] = {
        uint2(-1, 0), 
        uint2(+1, 0),
        uint2(0, -1),
        uint2(0, +1)
    };
    float height[4] = {0, 0, 0, 0};

    int i;
    [unroll]
    for(i = 0 ; i < 4 ; i++)
    {
        uint2 offset = TexCord + uint2(dUV[i]);
        offset.x = clamp(offset.x, 0, DimX - 1);
        offset.y = clamp(offset.y, 0, DimZ - 1);
        height[i] = TerrainHeightMap[offset].r * HeightScaler;
    }

    float StrideX = ((TexCord.x == 0) || (TexCord.x == DimX - 1)) ? 1.f : 2.f;
    float StrideZ = ((TexCord.y == 0) || (TexCord.y == DimZ - 1)) ? 1.f : 2.f;
    float HeightDiffX = (height[1] - height[0]);
    float HeightDiffZ = (height[3] - height[2]);

    float3 Tangent = normalize(float3(StrideX, HeightDiffX, 0));    // x
    float3 Bitangent = normalize(float3(0, HeightDiffZ, StrideZ));  // z
    float3 Normal = normalize(cross(Bitangent, Tangent));           // y
    return Normal;
}
#endif