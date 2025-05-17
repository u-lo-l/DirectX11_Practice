#pragma once
#include "Projection.h"

class Perspective final : public Projection
{
public:
	Perspective(float InWidth, float InHeight, float InNear, float InFar, float InFOV);
	void Set(float InWidth, float InHeight, float InNear, float InFar, float InFOV) override;
	~Perspective() override = default;


private:
	float Aspect = 0;
};
