#pragma once
#include "Renders/Transform.h"

class Projection;

class Projector
{
private:
	constexpr static int VS_CONST_DECAL_PROJECTOR = 9;
public:
	Projector(const wstring & InTextureFile, float InWidth, float InHeight, float InNear, float InFar);
	~Projector();

	void Tick();
	void Render();

	void SetPosition(const Vector & InPosition) const;
	void SetScale(const Vector & InScale) const;
	void SetRotation(const Vector & InEulerDegree) const;
	
	const Matrix & GetViewMatrix() const { return ProjectorData.View; }
	const Matrix & GetProjectionMatrix() const { return ProjectorData.Projection; }
	
	Texture* GetTexture() const { return DecalTexture; }

private:
	struct ProjectorDesc
	{
		Matrix View;
		Matrix Projection;
	} ProjectorData;
	ConstantBuffer * ProjectorCBuffer_VS;
	
private:
	float Width = 10.0f, Height = 10.0f;
	float Near = 1.0f;
	float Far = 25.0f;
	float FOV = 0.25f;

	Texture * DecalTexture;
	Transform * World;
	Projection * Proj;
};
