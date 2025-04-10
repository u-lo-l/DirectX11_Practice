#pragma once
#include "Projection.h"

class Orthographic final : public Projection
{
public:
	Orthographic(float InWidth, float InHeight, float InNear, float InFar);
	void Set(float InWidth, float InHeight, float InNear, float InFar, float InFOV = 0) override;
	~Orthographic() override = default;
};
