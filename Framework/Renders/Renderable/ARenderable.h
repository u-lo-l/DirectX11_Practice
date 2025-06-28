#pragma once

class RenderingShader;
class Material;

class ARenderable
{
protected:
	ARenderable();
public:
	virtual ~ARenderable();
	Transform * GetTransform() const { return Tf; }
	void Tick() const;
	void Render(const RenderingShader* InShader, int& DrawCallCount) const;
	
	void CreateVertexBuffer(void * InData, int InCount, int InStride);
	void CreateIndexBuffer(UINT* InData, int InCount);
	void CreateInstanceBuffer(void * InData, int InCount, int InStride);

	void SetTransform(const Matrix & InMatrix) const;
	const string & GetName() const;

	const Material * GetMaterial() const;
	const ID3D11InputLayout * GetInputLayout() const;
	const RenderingShader * GetShader() const;
	
	const VertexBuffer * GetVertexBuffer() const;
	const IndexBuffer * GetIndexBuffer() const;
	const InstanceBuffer * GetInstanceBuffer() const;

	virtual void BindResources() const = 0;

protected:
	void SetName(const string & InName);
	void SetMaterial(const Material * InMaterial);
	void SetInputLayOut(ID3D11InputLayout * InInputLayout);
	void SetShader();
	void BindBuffer() const;
	// TODO : Change To unique_ptr
	Transform * Tf = nullptr;
	string Name;
	// TODO : Change To shared_ptr
	const RenderingShader * Shader = nullptr;
private:
	// TODO : Change To shared_ptr
	const Material * Mat = nullptr;
	// TODO : Change To shared_ptr
	const ID3D11InputLayout * VertexInputLayout = nullptr;
	// TODO : Change To unique_ptr
	VertexBuffer * VBuffer = nullptr;
	// TODO : Change To unique_ptr
	IndexBuffer * IBuffer = nullptr;
	// TODO : Change To unique_ptr
	InstanceBuffer * InstBuffer = nullptr;
};
