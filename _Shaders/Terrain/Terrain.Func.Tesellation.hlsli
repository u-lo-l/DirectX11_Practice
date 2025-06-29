#ifndef __TERRAIN_FUNC_TESELLATION_HLSLI__
#define __TERRAIN_FUNC_TESELLATION_HLSLI__

# define DOMAIN "quad"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4
# ifndef MAX_TESS_FACTOR
#  define MAX_TESS_FACTOR 64
# endif

// Tessellation
const static float MinTessFactor = 1;
const static float MaxTessFactor = MAX_TESS_FACTOR;
const static float RDRatio = 4;

float CalculateTessellationFactor
(
	float4 Point1, // Camera Space Position1
	float4 Point2, // Camera Space Position2
	float ScreenDistance,
	float ScreenDiagonal,
	float RDRatio,
	float2 LODRange
)
{
    float L = ScreenDistance;
    float3 Center = ((Point1 + Point2) * 0.5).xyz;
    float R = length(Point2.xyz - Point1.xyz) * 0.5;
    float Distance = length(Center);
    float Dy = sqrt(Center.y * Center.y + Center.z * Center.z);
    float Dx = sqrt(Center.x * Center.x + Center.z * Center.z);
    float Ssd = 0;
    float TessRatio = 0;

    [flatten]
    if (Distance < 10.f || Distance * RDRatio < R)
    {
        return 1; // MAX
    }

    float PhiY = asin(saturate(RDRatio * R / Dy) / RDRatio);
    float PhiX = asin(saturate(RDRatio * R / Dx) / RDRatio);
    float LODNear = LODRange.x;
    float LODFar = LODRange.y;

# if defined (TYPE01)
    // SSD-Based
    const float MaxScreenDiagonal = (ScreenDiagonal) / LODFar;
    float ThetaY = atan2(Center.y , Center.z);
    float ThetaX = atan2(Center.x , Center.z);
    float SsdY = tan(ThetaY + PhiY) - tan(ThetaY - PhiY);
    float SsdX = tan(ThetaX + PhiX) - tan(ThetaX - PhiX);
    Ssd = pow(L * SsdX, 2) + pow(L * SsdY, 2);
    TessRatio = pow(saturate((sqrt(Ssd) / (MaxScreenDiagonal))), LODNear);

# elif defined (TYPE02) // Better
    // Approax SSD-Based
    const float MaxScreenDiagonal = (ScreenDiagonal) / LODFar;
    float SsdY = 2 * tan(PhiY);
    float SsdX = 2 * tan(PhiX);
    Ssd = pow(L * SsdX, 2) + pow(L * SsdY, 2);
    TessRatio = pow(saturate((sqrt(Ssd) / (MaxScreenDiagonal))), LODNear);

# elif defined (TYPE03)

    float D = min(length(Center), LODFar);
    TessRatio = saturate((LODFar - D) / (LODFar - LODNear));

# else
#  error "Either SSD Type must be defined."
# endif

    return TessRatio;
}

float CalculateDensity
(
	float3 PatchPosition0,
	float3 PatchPosition1,
	float3 PatchPosition2,
	float3 PatchPosition3
)
{
    float3 PatchNormal = normalize(cross(PatchPosition2 - PatchPosition0, PatchPosition3 - PatchPosition1));
    return 1 - saturate(PatchNormal.y);
}


#endif