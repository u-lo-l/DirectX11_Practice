#pragma once

class Camera
{
	static const Vector WORLD_FORWARD;
	static const Vector WORLD_UP;
	static const Vector WORLD_RIGHT;
public:
	Camera();
	~Camera();
	void Tick();

public:
	const Vector& GetPosition() const;
	const Matrix& GetViewMatrix() const;
	void SetPosition(float X, float Y, float Z);
	void SetPosition(const Vector & Vec);

	auto GetEulerAngle() const -> const Vector&;
	void SetRotation(float r, float p, float y);
	void SetRotation(const Vector & InEuler);

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
	const Vector& At() const;
};