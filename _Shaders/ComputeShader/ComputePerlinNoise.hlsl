#ifndef __COMPUTE_PERLIN_NOISE_HLSL__
#define __COMPUTE_PERLIN_NOISE_HLSL__
# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif
#define SMOOTH(x) ((x) * (x) * (x) * ((x) * ((x) * 6.0f - 15.0f) + 10.0f))

// https://en.wikipedia.org/wiki/Perlin_noise
const static int permutation[512] = {
                                      151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
                                      140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
                                      247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
                                      57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
                                      74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
                                      60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
                                      65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
                                      200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
                                      52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
                                      207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
                                      119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
                                      129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
                                      218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
                                      81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
                                      184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
                                      222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180,
                                      
                                      151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
                                      140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
                                      247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
                                      57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
                                      74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
                                      60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
                                      65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
                                      200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
                                      52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
                                      207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
                                      119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
                                      129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
                                      218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
                                      81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
                                      184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
                                      222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180,
                                    };
const static float2 gradients2D[8] = {
                                        normalize(float2(1, 1)), normalize(float2(-1, 1)), normalize(float2(1, -1)), normalize(float2(-1, -1)),
                                        normalize(float2(1, 0)), normalize(float2(-1, 0)), normalize(float2(0, 1)), normalize(float2(0, -1))
                                     };
RWTexture2D<float> Output : register(u0);

float PerlinNoise2D(float2 P);
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
    uint2 TexCoord = DTid.xy;
    uint2 TextureSize;
    Output.GetDimensions(TextureSize.x, TextureSize.y);

    if (TexCoord.x >= TextureSize.x || TexCoord.y >= TextureSize.y)
    {   
        return;
    }

    float frequency = 0.01f; // 예시 주파수: 노이즈 특징의 크기를 조절합니다. 이 값을 변경해 보세요.
    float2 noiseCoord = float2((float)TexCoord.x * frequency, (float)TexCoord.y * frequency);

    float noiseValue = PerlinNoise2D(noiseCoord);

    Output[TexCoord] = (noiseValue + 1.0f) * 0.5f; // -1~1 to 0~1
}

uint hash(uint x, uint y);
float2 getGradient(uint h);
float PerlinNoise2D(float2 UV)
{
    float2 CornerPosition[4];
    CornerPosition[0] = floor(UV);                   // top-left
    CornerPosition[1] = CornerPosition[0] + float2(1.0f, 0.0f);    // top-right
    CornerPosition[2] = CornerPosition[0] + float2(0.0f, 1.0f);    // bottom-left
    CornerPosition[3] = CornerPosition[0] + float2(1.0f, 1.0f);    // bottom-right

    // Get the integer coordinates (as unsigned integers for hashing)
    uint x0 = (uint)CornerPosition[0].x;
    uint y0 = (uint)CornerPosition[0].y;
    uint x1 = x0 + 1;
    uint y1 = y0 + 1;

    // Calculate the vectors from each corner to point P

    float2 VectorFromCorner[4];
    int i;
    [unroll] for (i = 0 ; i < 4 ; i++)
        VectorFromCorner[i] = UV - CornerPosition[i];

    // Get the gradient vectors for each corner using the hashing and lookup tables
    float2 Gradients[4];
    [unroll] for (i = 0 ; i < 4 ; i++)
        Gradients[i] = getGradient(hash((uint)CornerPosition[i].x, (uint)CornerPosition[i].y));

    // Calculate the dot product of the corner-to-P vector and the gradient vector for each corner
    // This gives the "influence" of each corner's gradient at point P
    float Dot[4];
    [unroll] for (i = 0 ; i < 4 ; i++)
        Dot[i] = dot(VectorFromCorner[i], Gradients[i]);

    float2 frac = (UV - CornerPosition[0]) / 1 ; // UV in GridSpace (GridSize : 1 x 1)
    float2 smoothedFrac = float2(SMOOTH(frac.x), SMOOTH(frac.y));

    // bilinear interp
    float interp_bottom = lerp(Dot[0], Dot[1], smoothedFrac.x);
    float interp_top = lerp(Dot[2], Dot[3], smoothedFrac.x);
    float final_noise = lerp(interp_bottom, interp_top, smoothedFrac.y);

    return final_noise;
}

uint hash(uint x, uint y)
{
    return permutation[permutation[x & 255] + (y & 255)];
}
float2 getGradient(uint h)
{
    return gradients2D[h & 7];
}
#endif