#pragma once
#include "ViewPort/ViewPort.h"

class Camera;
class Shadow;
class Perspective;
class GlobalViewProjectionCBuffer;

class Context
{
private:
	using ThisClass = Context;

	
public:
	static void Create();
	static void Destroy();
	static Context* Get();
	void Tick();
	void Render() const;
	void ResizeScreen();

	Camera * GetCamera() const;// {return MainCamera;}
	const Vector & GetLightDirection() const;// {return LightDirection;}
	const Color & GetLightColor() const;
	GlobalViewProjectionCBuffer * GetViewProjectionCBuffer() const;// { return VP_CBuffer_VS; }

private:
	static Context * Instance;

private:
	Context();
	~Context();
	
public:
	Shadow * GetShadowMap() const;
	Matrix GetViewMatrix() const;
	Matrix GetProjectionMatrix() const;
private:
	Camera * MainCamera;
	ViewPort * Vp;
	GlobalViewProjectionCBuffer * VP_CBuffer_VS = nullptr;
	Vector LightDirection = {1, -1, 1};
	Color LightColor = {1,1,1,1};
	Shadow * ShadowMap = nullptr;
};
