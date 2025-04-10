#include "framework.h"
#include "Orthographic.h"

Orthographic::Orthographic(float InWidth, float InHeight, float InNear, float InFar)
	: Projection(InWidth, InHeight, InNear, InFar, 0)
{
	ProjectionMatrix = Matrix::CreateOrthographic(InWidth, InHeight, InNear, InFar);
}

void Orthographic::Set(float InWidth, float InHeight, float InNear, float InFar, float InFOV)
{
	Projection::Set(InWidth, InHeight, InNear, InFar, 0);
	ProjectionMatrix = Matrix::CreateOrthographic(InWidth, InHeight, InNear, InFar);
}
