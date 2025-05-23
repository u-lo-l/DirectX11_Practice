﻿#pragma once

class Projection
{
public:
	Projection(float InWidth, float InHeight, float InNear, float InFar, float InFOV);
	virtual void Set(float InWidth, float InHeight, float InNear, float InFar, float InFOV);
	virtual ~Projection() = 0;
	const Matrix & GetMatrix() const { return ProjectionMatrix; }
	float GetFOV() const { return FOV; }
	float GetNear() const { return Near; }
	float GetFar() const { return Far; }
	float GetAspect() const { return Aspect; }
protected:
	Projection();

	float Width = 0.0f, Height = 0.0f;
	float Near = 0.0f, Far = 0.0f;
	float FOV = 0.0f;
	float Aspect = 0;
	Matrix ProjectionMatrix;
};
