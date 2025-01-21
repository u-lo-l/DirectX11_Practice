#pragma once

/**
 * CBuffer에 넘어갈 내용이 여기로 들어간다.
 */
class ConstantDataBinder
{
private:
	static UINT Count;
public:
	explicit ConstantDataBinder(Shader * InDrawer);
	~ConstantDataBinder();

	void Tick();
	void BindToGPU() const;
	
private:
	struct ContextDesc
	{
		Matrix View;
		Matrix ViewInv;
		Matrix Projection;
		Matrix ViewProjection;
		Vector LightDirection;
		float Padding;
	};

	ContextDesc ContextDescData;

	ConstantBuffer * ContextBuffer;
	IECB_t * ContextECB;

	Shader * Drawer;
};
