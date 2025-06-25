#include "framework.h"
#include "ARenderable.h"

ARenderable::ARenderable()
{
	Tf = new Transform();
}

ARenderable::~ARenderable()
{
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(InstBuffer);
	SAFE_DELETE(Tf);
}

void ARenderable::Tick() const
{
	Tf->Tick();
}

void ARenderable::CreateVertexBuffer(void* InData, const int InCount, const int InStride)
{
	VBuffer = new VertexBuffer(InData, InCount, InStride, 0);
}

void ARenderable::CreateIndexBuffer(UINT* InData, const int InCount)
{
	IBuffer = new IndexBuffer(InData, InCount);
}

void ARenderable::CreateInstanceBuffer(void* InData, const int InCount, const int InStride)
{
	InstBuffer = new InstanceBuffer(InData, InCount, InStride);
}

void ARenderable::SetTransform(const Matrix& InMatrix) const
{
	Tf->SetTransform(InMatrix);
}

void ARenderable::SetMaterial(const Material* InMaterial)
{
	this->Mat = InMaterial;
}

void ARenderable::SetInputLayOut(ID3D11InputLayout * InInputLayout)
{
	this->VertexInputLayout = InInputLayout;
}

const string& ARenderable::GetName() const
{
	return Name;
}

const Material* ARenderable::GetMaterial() const
{
	return this->Mat;
}

const VertexBuffer* ARenderable::GetVertexBuffer() const
{
	return this->VBuffer;
}

const IndexBuffer* ARenderable::GetIndexBuffer() const
{
	return this->IBuffer;
}

const InstanceBuffer* ARenderable::GetInstanceBuffer() const
{
	return this->InstBuffer;
}

void ARenderable::SetName(const string& InName)
{
	Name = InName;
}

const ID3D11InputLayout* ARenderable::GetInputLayout() const
{
	return this->VertexInputLayout;
}

const RenderingShader* ARenderable::GetShader() const
{
	return Shader;
}

void ARenderable::Render(const RenderingShader * InShader, int & DrawCallCount) const
{
	BindResources();
	RenderingShader::Draw(VBuffer, IBuffer, InstBuffer);
	DrawCallCount++;
}
