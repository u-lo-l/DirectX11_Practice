#pragma once
#include "Framework.h"

enum class EBlendType : uint8_t
{
	Opaque = 0,
	Additive,
	AlphaBlend,
	Multiply,
	Multiply2X
};

struct ParticleDesc // 모든 Particle의 공통 Constant Data
{
	Color MinColor;
	Color MaxColor;

	Vector Gravity;
	float Padding;

	Vector2D StartSize;
	Vector2D EndSize;

	Vector2D RotateSpeed;
	float Duration;				// Particle이 활성화 되고 실제로 나타나기 까지의 시간.
	float DurationModifier;

	float ColorAmount;
	float CurrentTime;		
	float Padding2[2];
};


struct ParticleData // Particle Configuration
{
public:
	EBlendType BlendType = EBlendType::Opaque;
public:
	bool bLoop = false;
	wstring TextureFileName;
public:
	UINT MaxParticle = 100;
public:
	float Duration = 1.0f;
	float DurationModifier = 0.0f;
public:
	float StartVelocity = 1;
	float EndVelocity = 1;

	float MinHorizontalVelocity = 0;
	float MaxHorizontalVelocity = 0;

	float MinVerticalVelocity = 0;
	float MaxVerticalVelocity = 0;
public:
	float MinRotateSpeed = 0;
	float MaxRotateSpeed = 0;

public:
	float MinStartSize = 100;
	float MaxStartSize = 100;

	float MinEndSize = 100;
	float MaxEndSize = 100;
public:
	Vector Gravity = Vector(0, 0, 0);
public:
	float ColorAmount = 1.0f;
	Color MinColor = Color(1, 1, 1, 1);
	Color MaxColor = Color(1, 1, 1, 1);
};