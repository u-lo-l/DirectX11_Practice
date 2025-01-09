#pragma once
#include "Buffers.h"

class Context
{
private:
	using ThisClass = Context;
	/**
	 * Shader의 Constant Buffer에 넣어 줄 Description
	 */
	struct Desc
	{
		Matrix View;
		Matrix ViewInv;
		Matrix Projection;
		Matrix ViewProjection;
		
		Vector LightDirection = {1, 1, 1};
		float Padding;
	};
public:
	static void Create();
	static void Destroy();
	static Context* Get();
	void Tick();
	void Render() const;
	void BindCBufferToGPU(const Shader * InDrawer = nullptr) const;
	void ResizeScreen();
public:
	__forceinline Camera * GetCamera() const {return MainCamera;}
	__forceinline const Vector & GetLightDirection() const {return this->Desc.LightDirection;}
private:
	static Context * Instance;
private:
	Context();
	~Context();
	
public:
	__forceinline Matrix GetViewMatrix() const { return MainCamera->GetViewMatrix(); }
	__forceinline Matrix GetProjectionMatrix() const { return this->Desc.Projection; }
private:
	Camera * MainCamera;
	D3D11_VIEWPORT * Viewport;

	Vector LightDirection = {-1, -1, +1};

	ThisClass::Desc Desc;

	ConstantBuffer * CBuffer;
};
