#pragma once

class SkyMaterial final : public Material
{
public:
	struct MaterialDesc
	{
		string Name = "M_Sky";
		string ShaderName = "Sky";
		wstring SkyTextureName =  L"Environments/SkyDawn.dds";
	};
	explicit SkyMaterial(const MaterialDesc & Desc);
	virtual ~SkyMaterial() override;
	virtual void Tick() override;
	virtual void BindToGpu(int RegisterIndex) const override;
private:
	Texture * Skybox = nullptr;
};
