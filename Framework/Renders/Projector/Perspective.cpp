#include "framework.h"
#include "Perspective.h"

Perspective::Perspective(float InWidth, float InHeight, float InNear, float InFar, float InFOV)
	: Projection(InWidth, InHeight, InNear, InFar, InFOV)
{
	Aspect = InWidth / InHeight;
	ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(InFOV, Aspect, InNear, InFar);
}

void Perspective::Set(float InWidth, float InHeight, float InNear, float InFar, float InFOV)
{
	__super::Set(InWidth, InHeight, InNear, InFar, InFOV);
	Aspect = InWidth / InHeight;
	ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(InFOV, Aspect, InNear, InFar);
}
