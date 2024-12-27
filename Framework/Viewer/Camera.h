// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
#pragma once
#include "../Core/Matrix.h"
class Camera
{
public:
	Camera();
	~Camera();
	void Tick();

public:
	const Vector& GetPosition() const;
	Matrix& GetViewMatrix();
	void SetPosition(float X, float Y, float Z);
	void SetPosition(const Vector & Vec);

	auto GetEulerAngle() const -> const Vector&;
	void SetRotation(float R, float P, float Y);
	void SetRotation(const Vector & InEuler);

	void SetMoveSpeed(float InSpeed);
	void SetRopSpeed(float InSpeed);
private :
	// Camera Translation
	//void SetMove();
	// Camera Rotation
	void SetViewMat();
	void SetRotationMat();

private :
	Matrix ViewMat;
	Matrix RotationMat;
	float MoveSpeed = 20.f;
	float RotationSpeed = 2.f;

private:
	Vector Position = {0.f, 0.f, 0.f};
	Vector EulerAngle = { 0.f,0.f, 0.f };
	Vector Forward = {0.f, 0.f, 1.f };
	Vector Up = { 0.f, 1.f, 0.f };
	Vector Right = { 1.f, 0.f, 0.f };
	Vector At() const;
};
