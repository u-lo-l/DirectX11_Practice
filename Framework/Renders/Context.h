#pragma once
#include "Buffers.h"

class Context
{
private:
	using ThisClass = Context;

	Matrix View;
	Matrix Projection;
	Vector LightDirection = {1, 1, 1};
	
public:
	static void Create();
	static void Destroy();
	static Context* Get();
	void Tick();
	void Render() const;
	void ResizeScreen();
public:
	__forceinline Camera * GetCamera() const {return MainCamera;}
	__forceinline const Vector & GetLightDirection() const {return LightDirection;}
private:
	static Context * Instance;
private:
	Context();
	~Context();
	
public:
	__forceinline Matrix GetViewMatrix() const { return MainCamera->GetViewMatrix(); }
	__forceinline Matrix GetProjectionMatrix() const { return Projection; }
private:
	Camera * MainCamera;
	D3D11_VIEWPORT * Viewport;

	ID3DX11EffectConstantBuffer * ECB = nullptr;
};
