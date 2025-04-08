#include "framework.h"
#include "PostEffect.h"

PostEffect::PostEffect(const wstring& InShaderFileName, RenderTarget * InRenderTarget)
	: PreRenderedTarget(InRenderTarget), VertexShader(nullptr),  GaussianBlurFactor{}
{
	const wstring ShaderPath = wstring(W_SHADER_PATH) + L"PostEffect/PostEffect.hlsl";
	Shader = new HlslShader<VertexType>(ShaderPath);
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
		8 * sizeof(float),
		false
	);

	const UINT ScreenWidth = static_cast<UINT>(D3D::GetDesc().Width);
	const UINT ScreenHeight = static_cast<UINT>(D3D::GetDesc().Height);
	Bloom_RT[Luminosity] = new RenderTarget(ScreenWidth, ScreenHeight);
	Bloom_RT[GaussianBlur_X] = new RenderTarget(ScreenWidth, ScreenHeight);
	Bloom_RT[GaussianBlur_Y] = new RenderTarget(ScreenWidth, ScreenHeight);
	Bloom_RT[Final] = new RenderTarget(ScreenWidth, ScreenHeight);
	Bloom_DS = new DepthStencil(ScreenWidth, ScreenHeight, false);

	UINT ShaderTarget = (UINT)ShaderType::VertexShader | (UINT)ShaderType::PixelShader;
	Bloom_Shader[0] = new HlslShader<VertexType>(ShaderPath, ShaderTarget, "VSMain", "PS_Bloom_Separate");
	Bloom_Shader[0]->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CHECK(SUCCEEDED(Bloom_Shader[0]->CreateSamplerState_Linear()));

	Bloom_Shader[1] = new HlslShader<VertexType>(ShaderPath, ShaderTarget, "VSMain", "PS_Bloom_Combine");
	Bloom_Shader[1]->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CHECK(SUCCEEDED(Bloom_Shader[1]->CreateSamplerState_Linear()));

	GaussianBlur_Shader[0] = new HlslShader<VertexType>(ShaderPath, ShaderTarget, "VSMain", "PS_GaussianBlur_X");
	GaussianBlur_Shader[0]->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CHECK(SUCCEEDED(GaussianBlur_Shader[0]->CreateSamplerState_Linear()));

	GaussianBlur_Shader[1] = new HlslShader<VertexType>(ShaderPath, ShaderTarget, "VSMain", "PS_GaussianBlur_Y");
	GaussianBlur_Shader[1]->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CHECK(SUCCEEDED(GaussianBlur_Shader[1]->CreateSamplerState_Linear()));
}

PostEffect::~PostEffect()
{
	SAFE_DELETE(Shader);
	SAFE_DELETE(VBuffer);
}

void PostEffect::Tick()
{
	int PostEffectIndex = static_cast<int>(PostEffectFactors[3]); 
	ImGui::SliderInt("PostEffect Index", &PostEffectIndex, 0, 12);
	PostEffectFactors[2] = sdt::SystemTimer::Get()->GetRunningTime();
	PostEffectFactors[3] = static_cast<float>(PostEffectIndex);
	PostEffectFactors_CBuffer_PS->UpdateData(PostEffectFactors, 8 * sizeof(float));
	if (PostEffectIndex == 12)
	{
		if (Keyboard::Get()->IsPressed(VK_SPACE))
		{
			Bloom_RT[Luminosity]->SaveTexture(L"Bright");
			Bloom_RT[GaussianBlur_X]->SaveTexture(L"Bright_BlurX");
			Bloom_RT[GaussianBlur_Y]->SaveTexture(L"Bright_BlurXY");
			Bloom_RT[Final]->SaveTexture(L"Final");
		}
	}
}

/*
 * PostEffet의 PreRender가 호출되는 시점은
 * 원본 렌더링이 다 끝난 이후다. InDiffuseSRV에는 Scene을 기본적으로 렌더링 한 결과 이미지가 담겨있다.
 */
void PostEffect::PreRender(ID3D11ShaderResourceView * InDiffuseSRV)
{
	if (static_cast<int>(PostEffectFactors[3]) == 12)
		return PreRender_Bloom(InDiffuseSRV);
}

void PostEffect::Render()
{
	ID3D11DeviceContext * Context = D3D::Get()->GetDeviceContext();
	
	VBuffer->BindToGPU();
	PostEffectFactors_CBuffer_PS->BindToGPU();
	ID3D11ShaderResourceView * FinalResultSRV;
	if (static_cast<int>(PostEffectFactors[3]) == 12)
		FinalResultSRV = *Bloom_RT[Final];
	else
		FinalResultSRV = *PreRenderedTarget;
	Context->PSSetShaderResources(0, 1, &FinalResultSRV);
	Shader->Draw(4);
}

void PostEffect::PostRender()
{
}

/*====================================================================================================================*/

// PASS 01 : BrightColor 추출
// PASS 02 : 추출된 BrightColor에 Blur_X 적용
// PASS 03 : Blur_X적용된 결과에 Blur_Y적용
// PASS 04 : 원본 이미지와 합성
void PostEffect::PreRender_Bloom(ID3D11ShaderResourceView * InOriginalTextureSRV)
{
	ID3D11DeviceContext * Context = D3D::Get()->GetDeviceContext();

	{
		// PASS 01
		//	사용 될 쉐이더 . Bloom_Shader[0]. .. 1
		//	사용될 Texture : InOriginalTextureSRV에의해 넘어온 이미지 .. 2
		//	사용할 Render Target : Bloom_RT[Luminosity] .. 3;
		Bloom_RT[Luminosity]->ClearRenderTarget();
		Bloom_DS->ClearDepthStencil();
		Bloom_RT[Luminosity]->SetRenderTarget(Bloom_DS); // -> 3
		
		ID3D11ShaderResourceView * SourceImageSRV = *PreRenderedTarget;
		Context->GenerateMips(SourceImageSRV);
		Context->PSSetShaderResources(0, 1, &SourceImageSRV); // -> 2
		VBuffer->BindToGPU();
		PostEffectFactors_CBuffer_PS->BindToGPU();
		
		Bloom_Shader[0]->Draw(4); // -> 1
	}
	{
		// PASS 02
		//	사용 될 쉐이더 . GaussianBlur_Shader[0].
		//	사용될 Texture : Bloom_RT[Luminosity]에 저장된 이미지
		//	사용할 Render Target : Bloom_RT[GaussianBlur_X];
		Bloom_RT[GaussianBlur_X]->ClearRenderTarget();
		Bloom_DS->ClearDepthStencil();
		Bloom_RT[GaussianBlur_X]->SetRenderTarget(Bloom_DS);
		
		ID3D11ShaderResourceView * SourceImageSRV = *Bloom_RT[Luminosity];
		Context->PSSetShaderResources(0, 1, &SourceImageSRV);
		
		VBuffer->BindToGPU();
		PostEffectFactors_CBuffer_PS->BindToGPU();
		
		GaussianBlur_Shader[0]->Draw(4);
	}
	{
		// PASS 03
		//	사용 될 쉐이더 . GaussianBlur_Shader[1].
		//	사용될 Texture : Bloom_RT[GaussianBlur_X]에 저장된 이미지
		//	사용할 Render Target : Bloom_RT[GaussianBlur_Y];
		Bloom_RT[GaussianBlur_Y]->ClearRenderTarget();
		Bloom_DS->ClearDepthStencil();
		Bloom_RT[GaussianBlur_Y]->SetRenderTarget(Bloom_DS);
		
		ID3D11ShaderResourceView * SourceImageSRV = *Bloom_RT[GaussianBlur_X];
		Context->PSSetShaderResources(0, 1, &SourceImageSRV);
		
		VBuffer->BindToGPU();
		PostEffectFactors_CBuffer_PS->BindToGPU();
		
		GaussianBlur_Shader[1]->Draw(4);
	}
	{
		// PASS 04
		//	사용 될 쉐이더 . PreRenderedTarget. PostEffect 호출한 상위의 RT. 즉 바로 직전 RT
		//		PreRender가 끝나고 Render에서 BackBufferRT의 이미지를 그린다.
		//	사용될 Texture : Bloom_RT[GaussianBlur_Y]에 저장된 이미지, 원본 이미지
		//	사용할 Render Target : Bloom_RT[Final];
		
		Bloom_RT[Final]->ClearRenderTarget();
		Bloom_DS->ClearDepthStencil();
		Bloom_RT[Final]->SetRenderTarget(Bloom_DS);
		
		ID3D11ShaderResourceView * OriginSRV = InOriginalTextureSRV;
		Context->PSSetShaderResources(0, 1, &OriginSRV);
		ID3D11ShaderResourceView * BloomSRV = *Bloom_RT[GaussianBlur_Y];
		Context->PSSetShaderResources(1, 1, &BloomSRV);
		
		VBuffer->BindToGPU();
		PostEffectFactors_CBuffer_PS->BindToGPU();
		Bloom_Shader[1]->Draw(4);
	}
}

void PostEffect::PreRender_GaussianBlur(ID3D11ShaderResourceView * InOriginalTextureSRV)
{
}
