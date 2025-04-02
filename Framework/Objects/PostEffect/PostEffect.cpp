#include "framework.h"
#include "PostEffect.h"

PostEffect::PostEffect(const wstring& InShaderFileName, ID3D11ShaderResourceView* InSRV)
	: SRV(InSRV)
{
	Shader = new HlslShader<VertexType>(wstring(W_SHADER_PATH) + L"PostEffect/PostEffect.hlsl");
	Shader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Linear()));
	CHECK(SUCCEEDED(Shader->CreateSamplerState_Anisotropic()));
	
	vector<VertexType> Vertices(6);
	Vertices[0].Position = {-1.f, -1.f, 0.0f};
	Vertices[1].Position = {-1.f, +1.f, 0.0f};
	Vertices[2].Position = {+1.f, -1.f, 0.0f};
	Vertices[3].Position = {+1.f, +1.f, 0.0f};

	VBuffer = new VertexBuffer(
		Vertices.data(),
		4,
		sizeof(VertexType)
	);

	PostEffectFactors[0] = 1 / D3D::GetDesc().Width;
	PostEffectFactors[1] = 1 / D3D::GetDesc().Height;
	PostEffectFactors[2] = sdt::SystemTimer::Get()->GetRunningTime();
	PostEffectFactors[4] = 10.f; // Radial Blur Radius
	PostEffectFactors[5] = 0.4f; // Radial Blur Amount
	PostEffectFactors[6] = 0.f;  // Radial Blur Center.X
	PostEffectFactors[7] = 0.f;  // Radial Blur Center.Y
	
	PostEffectFactors_CBuffer_PS = new ConstantBuffer(
		ShaderType::PixelShader,
		Slot_Const_PixelSize_PS,
		PostEffectFactors,
		"Pixel Size",
		8  * sizeof(float),
		false
	);
}

PostEffect::~PostEffect()
{
	SAFE_DELETE(Shader);
	SAFE_DELETE(VBuffer);
}

void PostEffect::Tick()
{
	int PostEffectIndex = static_cast<int>(PostEffectFactors[3]); 
	ImGui::SliderInt("PostEffect Index", &PostEffectIndex, 0, 10);
	PostEffectFactors[2] = sdt::SystemTimer::Get()->GetRunningTime();
	PostEffectFactors[3] = static_cast<float>(PostEffectIndex);
	PostEffectFactors_CBuffer_PS->UpdateData(PostEffectFactors, 8 * sizeof(float));
}

void PostEffect::Render()
{
	ID3D11DeviceContext * Context = D3D::Get()->GetDeviceContext();
	
	VBuffer->BindToGPU();
	PostEffectFactors_CBuffer_PS->BindToGPU();
	Context->PSSetShaderResources(0, 1, &SRV);
	Shader->Draw(4);
}
