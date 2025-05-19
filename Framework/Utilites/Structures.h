#pragma once

struct WVPDesc
{
	Matrix World = Matrix::Identity;
	Matrix View;
	Matrix Projection;
};

struct WVPIDesc
{
	Matrix World = Matrix::Identity;
	Matrix View;
	Matrix Projection;
	Matrix ViewInverse;
};

struct DirectionalLightDesc
{
	Color LightColor = Color::White;
	Vector LightDirection;
	float Padding;
};