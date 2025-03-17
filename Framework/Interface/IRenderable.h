#pragma once

struct WVPDesc
{
	Matrix View;
	Matrix Projection;
};
// 특정 카메라에 보일 수 있는 오브젝트
class IRenderable
{
protected:
	explicit IRenderable(Camera * InCamera = nullptr);
	virtual ~IRenderable();
	void SetCamera(Camera * InCamera);
	virtual void Tick();
	virtual void Render() const;
private:
	void UpdateCameraMatrix();
	WVPDesc WVP_Matrix;;
	ConstantBuffer * WVP_CBuffer = nullptr;
	Camera * TargetCamera = nullptr;
};
