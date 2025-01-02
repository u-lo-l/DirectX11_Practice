#pragma once

class Context
{
public:
	static void Create();
	static void Destroy();
	static Context* Get();
	void Tick() const;
	void Render() const;
	void ResizeScreen();
public:
	__forceinline class Camera * GetCamera() const {return MainCamera;}
	__forceinline const Vector & GetLightDirection() const {return LightDirection;}
private:
	static Context * Instance;
private:
	Context();
	~Context();
	
public:
	__forceinline Matrix GetViewMatrix() const { return MainCamera->GetViewMatrix(); }
	__forceinline Matrix GetProjectionMatrix() const { return ProjectionMat; }
private:
	Camera * MainCamera;
	Matrix ProjectionMat;
	D3D11_VIEWPORT * Viewport;

	Vector LightDirection = {-1, -1, +1};
};