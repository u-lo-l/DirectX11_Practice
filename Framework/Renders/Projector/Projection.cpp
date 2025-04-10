#include "framework.h"
#include "Projection.h"

Projection::Projection(float InWidth, float InHeight, float InNear, float InFar, float InFOV)
	: Width(InWidth), Height(InHeight), Near(InNear), Far(InFar), FOV(InFOV)
{
}

Projection::~Projection()
{
}

Projection::Projection()
{
	ProjectionMatrix = Matrix::Identity;
}

void Projection::Set(float InWidth, float InHeight, float InNear, float InFar, float InFOV)
{
	Width = InWidth;
	Height = InHeight;
	Near = InNear;
	Far = InFar;
	FOV = InFOV;
}
