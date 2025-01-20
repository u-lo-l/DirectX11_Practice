#pragma once

class ConstantDataBinder;

class Context
{
private:
	using ThisClass = Context;

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
	Camera * GetCamera() const {return MainCamera;}
	const Vector & GetLightDirection() const {return LightDirection;}
private:
	static Context * Instance;
private:
	Context();
	~Context();
	
public:
	const Matrix & GetViewMatrix() const { return MainCamera->GetViewMatrix(); }
	const Matrix & GetProjectionMatrix() const { return Projection; }
private:
	Camera * MainCamera;
	D3D11_VIEWPORT * Viewport;
public:
	ConstantDataBinder * CBBinder = nullptr;
};
