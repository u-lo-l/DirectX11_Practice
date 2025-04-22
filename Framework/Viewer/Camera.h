// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
#pragma once

class Camera
{
public:
	Camera();
	~Camera();
	void Tick();

public:
	const Vector& GetPosition() const;
	Matrix GetViewMatrix() const;
	Matrix GetProjectionMatrix() const;
	void SetPosition(float X, float Y, float Z) const;
	void SetPosition(const Vector & Vec) const;
	const Vector & GetEulerAngleInDegree() const;
	const Vector & GetEulerAngleInRadian() const;
	void SetRotation(float R, float P, float Y) const;
	void SetRotation(const Vector & InEulerRadian) const;

	void SetMoveSpeed(float InSpeed);
	void SetRopSpeed(float InSpeed);

	void SetPerspective(float Width, float Height, float Near, float Far, float VFOV) const;

private :
	Transform * Tf;
	Projection * Proj;
	float DefaultMoveSpeed = 20.f;
	float MaxMoveSpeed = 10000.f;
	float MoveSpeed = 20.f;
	float RotationSpeed = 1.f;

private:
	Vector At() const;
};
