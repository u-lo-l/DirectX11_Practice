#pragma once

class SkySphere : public ARenderable
{
	using VertexType = Vertex;
public:
	struct Desc
	{
		string Name = "Sky";
		string ShaderName = "Skybox";
	};
	explicit SkySphere(Desc InDesc);
	virtual ~SkySphere() override;
	virtual void BindResources() const override;
	void Tick();
private:
	void CreateVertices();
	void CreateIndices();
	SkyMaterial * Mat;
	vector<VertexType> Vertices = {};
	vector<UINT> Indices = {};
	Desc Info;
	float Radius = 1.f;
	UINT SliceCount = 32;
};
