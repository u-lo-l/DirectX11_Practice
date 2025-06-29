#ifndef __FOLIAGE_FUNC_HLSLI__
#define __FOLIAGE_FUNC_HLSLI__

#include "./Foliage.Param.hlsli"

static const float2 UVs[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

void PointToPatch
(
	VS_OUTPUT Input,
	inout TriangleStream<GS_OUTPUT> stream,
	float4 Positions[4],
	float  MipLevel
)
{
	GS_OUTPUT output;

	[unroll(4)]
	for (int i = 0; i < 4; i++)
	{
		output.Position = Positions[i];
		output.WPosition = Input.WPosition.xyz;
		output.UV = Input.UV;
		output.TexCord = UVs[i];
		output.MapIndex = Input.MapIndex;
		output.MipLevel = MipLevel;
		stream.Append(output);
	}
}

void Foliage_Billboard
(
	VS_OUTPUT Input,
	inout TriangleStream<GS_OUTPUT> stream,
	float3 CameraWorldPosition,
	matrix ViewProjection,
	float MipLevel
)
{
	// float2 Scale = float2(10,20);
	float2 Scale = Input.Scale;

	float3 up = float3(0, 1, 0);
	float3 forward = normalize(Input.WPosition.xyz - CameraWorldPosition);
	float3 right = normalize(cross(up, forward));

	Input.WPosition.y += Scale.y * 0.3f;
	float2 size = Scale * 0.5f;

	float4 position[4];
	position[0] = mul(float4(Input.WPosition.xyz - size.x * right - size.y * up, 1), ViewProjection);
	position[1] = mul(float4(Input.WPosition.xyz - size.x * right + size.y * up, 1), ViewProjection);
	position[2] = mul(float4(Input.WPosition.xyz + size.x * right - size.y * up, 1), ViewProjection);
	position[3] = mul(float4(Input.WPosition.xyz + size.x * right + size.y * up, 1), ViewProjection);

	PointToPatch(Input, stream, position, MipLevel);
}

void Foliage_CrossQuad
(
	VS_OUTPUT Input,
	inout TriangleStream<GS_OUTPUT> stream,
	float3 CameraWorldPosition,
	matrix ViewProjection,
	float MipLevel
)
{
	const float2 Scale = Input.Scale;

	Input.WPosition.y += Scale.y * 0.3f;
	float2 size = Scale * 0.5f;
	float c45 = size.x * cos(radians(45));
	float s45 = size.x * sin(radians(45));

	float2 uv[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

	float4 position[2][4];
	position[0][0] = mul(Input.WPosition + float4(-c45, -size.y, -s45, 0), ViewProjection);
	position[0][1] = mul(Input.WPosition + float4(-c45, +size.y, -s45, 0), ViewProjection);
	position[0][2] = mul(Input.WPosition + float4( c45, -size.y, +s45, 0), ViewProjection);
	position[0][3] = mul(Input.WPosition + float4( c45, +size.y, +s45, 0), ViewProjection);

	position[1][0] = mul(Input.WPosition + float4(+c45, -size.y, -s45, 0), ViewProjection);
	position[1][1] = mul(Input.WPosition + float4(+c45, +size.y, -s45, 0), ViewProjection);
	position[1][2] = mul(Input.WPosition + float4(-c45, -size.y, +s45, 0), ViewProjection);
	position[1][3] = mul(Input.WPosition + float4(-c45, +size.y, +s45, 0), ViewProjection);

	[unroll]
	for( int i = 0 ; i < 2 ; i++)
	{
		PointToPatch(Input, stream, position[i], MipLevel);
		stream.RestartStrip();
	}
}

void Foliage_Triangle
(
	VS_OUTPUT Input,
	inout TriangleStream<GS_OUTPUT> stream,
	float3 CameraWorldPosition,
	matrix ViewProjection,
	float MipLevel
)
{
	const float2 Scale = Input.Scale;

	Input.WPosition.y += Scale.y * 0.3f;
	float2 size = Scale * 0.5f;
	float2 uv[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

	float c60 = size.x * cos(radians(60));
	float s60 = size.x * sin(radians(60));
	float c30 = size.x * cos(radians(30));
	float s30 = size.x * sin(radians(30));

	float4 position[3][4];
	float d = 1.f / 5.f;
	position[0][0] = mul(Input.WPosition + float4(-size.x, -size.y, d * size.x, 0), ViewProjection);
	position[0][1] = mul(Input.WPosition + float4(-size.x, +size.y, d * size.x, 0), ViewProjection);
	position[0][2] = mul(Input.WPosition + float4(+size.x, -size.y, d * size.x, 0), ViewProjection);
	position[0][3] = mul(Input.WPosition + float4(+size.x, +size.y, d * size.x, 0), ViewProjection);

	position[1][0] = mul(Input.WPosition + float4(-c60, -size.y, -s60, 0) + d * float4(c30, 0, -s30, 0), ViewProjection);
	position[1][1] = mul(Input.WPosition + float4(-c60, +size.y, -s60, 0) + d * float4(c30, 0, -s30, 0), ViewProjection);
	position[1][2] = mul(Input.WPosition + float4( c60, -size.y, +s60, 0) + d * float4(c30, 0, -s30, 0), ViewProjection);
	position[1][3] = mul(Input.WPosition + float4( c60, +size.y, +s60, 0) + d * float4(c30, 0, -s30, 0), ViewProjection);

	position[2][0] = mul(Input.WPosition + float4(+c60, -size.y, -s60, 0) + d * float4(-c30, 0, -s30, 0), ViewProjection);
	position[2][1] = mul(Input.WPosition + float4(+c60, +size.y, -s60, 0) + d * float4(-c30, 0, -s30, 0), ViewProjection);
	position[2][2] = mul(Input.WPosition + float4(-c60, -size.y, +s60, 0) + d * float4(-c30, 0, -s30, 0), ViewProjection);
	position[2][3] = mul(Input.WPosition + float4(-c60, +size.y, +s60, 0) + d * float4(-c30, 0, -s30, 0), ViewProjection);

	[unroll]
	for( int i = 0 ; i < 3 ; i++)
	{
		PointToPatch(Input, stream, position[i], MipLevel);
		stream.RestartStrip();
	}
}

bool CheckVisibility
(
	float3 ViewSpacePosition,
	float4 WorldPosition,
	float  FoliageDensity,
	float3 TerrainNormal,
	float2 AltitudeRange,
	float Random
)
{
	// check backward
	[flatten] if (ViewSpacePosition.z <= 0.f)
		return false;

	// check perlin noise
	[flatten] if (FoliageDensity < Random)
		return false;

	// check slope
	float SlopeThreshold = cos(radians(30.f));
	float D = dot(TerrainNormal, float3(0, 0, 1));
	[flatten] if (D < SlopeThreshold)
		return false;

	// check altitude
	const float Altitude = WorldPosition.y;
	const float MinAltitude = AltitudeRange[0];
	const float MaxAltitude = AltitudeRange[1];
	[flatten] if (Altitude < MinAltitude || Altitude >= MaxAltitude)
		return false;

	// altitude blending
	float AltitudeBlendingHeight = (MaxAltitude - MinAltitude) * 0.25f;
	float AltitudeLow_ = MinAltitude + AltitudeBlendingHeight;
	float AltitudeHigh_ = MaxAltitude - AltitudeBlendingHeight;

	float AltitudeWeight = 1.f;
	[flatten]
	if (Altitude < AltitudeLow_)
		AltitudeWeight = lerp(0.0f, 1.0f, (Altitude - MinAltitude) / AltitudeBlendingHeight);
	else if (Altitude > AltitudeHigh_)
		AltitudeWeight = lerp(1.0f, 0.0f, (Altitude - AltitudeHigh_) / AltitudeBlendingHeight);

	return AltitudeWeight >= Random;
}


#endif