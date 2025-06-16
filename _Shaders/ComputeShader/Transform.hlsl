#ifndef __TRANSFORM_HLSL__
#define __TRANSFORM_HLSL__

#ifndef PI
# define PI (3.14159274f)
#endif
#ifndef EPSILON
# define EPSILON (1e-7f)
#endif 

struct BoneMatrix_t
{
	matrix M;
};

struct BoneTRS_t
{
	float3 Translation;
	float4 Rotation;
	float3 Scale;
};


float4 slerp(float4 a, float4 b, float t);
BoneTRS_t lerp(in BoneTRS_t a, in BoneTRS_t b, float t);
BoneTRS_t barycentric(in BoneTRS_t a, in BoneTRS_t b, in BoneTRS_t c, float w0, float w1, float w2);

matrix ToMatrix(in BoneTRS_t TRS)
{
	const float3 Position = (float3)TRS.Translation;
	const float4 Rotation = (float4)TRS.Rotation;
	const float3 Scale    = (float3)TRS.Scale;

	float x2 = Rotation.x + Rotation.x;
    float y2 = Rotation.y + Rotation.y;
    float z2 = Rotation.z + Rotation.z;

    float xx2 = Rotation.x * x2;
    float yy2 = Rotation.y * y2;
    float zz2 = Rotation.z * z2;

    float yz2 = Rotation.y * z2;
    float wx2 = Rotation.w * x2;
    float xy2 = Rotation.x * y2;
    float wz2 = Rotation.w * z2;
    float xz2 = Rotation.x * z2;
    float wy2 = Rotation.w * y2;

    return matrix(
        (1.0 - yy2 - zz2) * Scale.x, (xy2 + wz2) * Scale.x,       (xz2 - wy2) * Scale.x,       0.0,
        (xy2 - wz2) * Scale.y,       (1.0 - xx2 - zz2) * Scale.y, (yz2 + wx2) * Scale.y,       0.0,
        (xz2 + wy2) * Scale.z,       (yz2 - wx2) * Scale.z,       (1.0 - xx2 - yy2) * Scale.z, 0.0,
        Position.x,                  Position.y,                  Position.z,                  1.0
    );
}

BoneTRS_t GetBoneTRS(in Texture2D<float4> Animation, uint BoneIndex, uint Frame)
{
	BoneTRS_t Result;
	Result.Translation = Animation.Load(int3(BoneIndex * 3 + 0, Frame, 0)).xyz;
	Result.Rotation    = Animation.Load(int3(BoneIndex * 3 + 1, Frame, 0)).yzwx; // (wxyz to xywz)
	Result.Scale       = Animation.Load(int3(BoneIndex * 3 + 2, Frame, 0)).xyz;
	return Result;
}

BoneTRS_t GetInterpolatedBoneTRS(in Texture2D<float4> Animation, uint BoneIndex, uint CurrentFrame, uint NextFrame, float t)
{
	BoneTRS_t Curr, Next;
	Curr = GetBoneTRS(Animation, BoneIndex, CurrentFrame);
	Next = GetBoneTRS(Animation, BoneIndex, NextFrame);
	return lerp(Curr, Next, t);
}

BoneTRS_t lerp(in BoneTRS_t a, in BoneTRS_t b, float t)
{
	if (t == 0)
		return a;
	else if (t == 1)
		return b;
	
	BoneTRS_t Result;
	Result.Translation = lerp(a.Translation, b.Translation, t);
	Result.Rotation = slerp(a.Rotation, b.Rotation, t);
	Result.Scale = lerp(a.Scale, b.Scale, t);

	return Result;
}

float4 slerp(float4 a, float4 b, float t)
{
	if (t == 0)
		return a;
	else if (t == 1)
		return b;
	
	float4 q0 = normalize(a);
	float4 q1 = normalize(b);
	float dot_product = dot(q0, q1);

	if (dot_product < 0.0f)
	{
		q1 = -q1;
		dot_product = -dot_product;
	}
	if (dot_product > 1.f - EPSILON)
	{
		const float4 a = lerp(q0, q1, t);
		return normalize(a);
	}

	float theta = acos(dot_product);
	float sin_theta = sin(theta);
	float weight0 = sin((1.0f - t) * theta) / sin_theta;
	float weight1 = sin(t * theta) / sin_theta;
	return normalize(weight0 * q0 + weight1 * q1);
}

// https://learn.microsoft.com/en-us/previous-versions/windows/desktop/bb281615(v=vs.85)
// the BaryCentric method implements the following series of spherical linear interpolation
BoneTRS_t barycentric
(
	in BoneTRS_t a,
	in BoneTRS_t b,
	in BoneTRS_t c,
	float w0,
	float w1,
	float w2
)
{
	[flatten]
	if (w0 == 1)
		return a;
	else if (w2 == 0)
		return lerp(a, b, w1);
	else
	{
		BoneTRS_t Result;
		Result.Translation = a.Translation * w0 + b.Translation * w1 + c.Translation * w2;
		Result.Scale = a.Scale * w0 + b.Scale * w1 + c.Scale * w2;
		float4 QuatTemp = slerp(a.Rotation, b.Rotation, w1 / (w0 + w1));
		Result.Rotation = slerp(QuatTemp, c.Rotation, w2);
		return Result;
	}
}
#endif