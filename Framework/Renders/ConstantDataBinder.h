#pragma once

struct ViewProjectionDesc
{
	Matrix View;
	Matrix ViewInv;
	Matrix Projection;
};

class GlobalViewProjectionCBuffer
{
public:
	explicit GlobalViewProjectionCBuffer();
	~GlobalViewProjectionCBuffer();

	void Tick();
	void BindToGPU() const;

private:
	struct LightDirectionDesc
	{
		Vector LightDirection;
		float Padding;
	};

	ViewProjectionDesc ViewProjectionData {};
	LightDirectionDesc LightDirectionData {};
	
	ConstantBuffer * ViewProjectionBuffer;
	ConstantBuffer * LightDirectionBuffer;
};
