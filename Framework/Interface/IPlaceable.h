#pragma once

// Scene에 배치될 수 있는 Object
class IPlaceable
{
public:
	IPlaceable(const Matrix * InMatrix = nullptr);
	virtual ~IPlaceable() = default;
protected:
	virtual void SetPosition(const Vector & Position);
	virtual void SetRotation(const Quaternion& Rotation);
	virtual void SetScale(const Vector & Scale);
	const Transform * GetTransform() const;
private:
	Transform * TF = nullptr;
};
