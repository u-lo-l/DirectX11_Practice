#pragma once

class GlobalViewProjectionCBuffer
{
private:
	static UINT Count;
public:
	GlobalViewProjectionCBuffer();
	~GlobalViewProjectionCBuffer();

	void Tick();
	void BindToGPU() const;
	
private:
	// struct ContextDesc
	// {
	// 	Matrix View;
	// 	Matrix ViewInv;
	// 	Matrix Projection;
	// 	Matrix ViewProjection;
	// 	Vector LightDirection;
	// 	float Padding;
	// };
	// ContextDesc ContextDescData;

	struct ViewProjectionDesc
	{
		Matrix View;
		Matrix Projection;
		Vector LightDirection;
		float Padding;
	};

	ViewProjectionDesc ViewProjectionData;
	
	ConstantBuffer * ViewProjectionBuffer;
};
