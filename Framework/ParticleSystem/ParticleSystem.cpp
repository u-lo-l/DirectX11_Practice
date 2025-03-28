#include "framework.h"
#include "ParticleSystem.h"

const wstring ParticleSystem::ShaderName = L"Particle/42_ParticleSystem.hlsl";

ParticleSystem::ParticleSystem(const wstring & InFileName)
{
	SetupParticle(wstring(W_TEXTURE_PATH) + L"Particles/" + InFileName + L".json");
	
	WorldTF = new Transform();
	Vertices.resize(Data.MaxParticle);
	CircularVBuffer = new CircularVertexBuffer(
		Vertices.data(),
		Data.MaxParticle,
		sizeof(VertexType)
	);
	ParticleDescCBuffer_VS = new ConstantBuffer(
		::ShaderType::VertexShader,
	Const_Slot_VS_ParticleSetting,
		&Data,
		"Particle Setting Data_VS",
		sizeof(ParticleDesc),
		false
	);
	ParticleDescCBuffer_GS = new ConstantBuffer(
		::ShaderType::GeometryShader,
		Const_Slot_GS_ParticleSetting,
		&Data,
		"Particle Setting Data_VS",
		sizeof(ParticleDesc),
		false
	);

	UINT UsingShaderTypeFlag = 0;
	UsingShaderTypeFlag |= static_cast<UINT>(ShaderType::VertexShader);
	UsingShaderTypeFlag |= static_cast<UINT>(ShaderType::PixelShader);
	UsingShaderTypeFlag |= static_cast<UINT>(ShaderType::GeometryShader);
	ParticleShader = new ShaderTypeName(ShaderName, UsingShaderTypeFlag);
	ParticleShader->SetTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	CHECK(SUCCEEDED(ParticleShader->CreateRasterizerState_Solid()));
	CHECK(SUCCEEDED(ParticleShader->CreateBlendState_Additive()));
	CHECK(SUCCEEDED(ParticleShader->CreateDepthStencilState_Particle()));
	Reset();
}

ParticleSystem::~ParticleSystem()
{	
	SAFE_DELETE(WorldTF);
	SAFE_DELETE(ParticleTexture);
	SAFE_DELETE(ParticleShader);
	SAFE_DELETE(ParticleDescCBuffer_VS);
	SAFE_DELETE(CircularVBuffer);
 }

void ParticleSystem::Tick()
{
	ElapsedTime += sdt::SystemTimer::Get()->GetDeltaTime();
	UpdateParticleVertices();
	CheckExpiredParticle();
	
	if (ActiveIndex == CpuRearIndex)
		return ;

	WorldTF->Tick();
	ParticleSetting.CurrentTime = ElapsedTime;
	ParticleDescCBuffer_VS->UpdateData(&ParticleSetting, sizeof(ParticleDesc));
	ParticleDescCBuffer_GS->UpdateData(&ParticleSetting, sizeof(ParticleDesc));
	 if (ActiveIndex == CpuRearIndex)
	 {
		CpuRearIndex = GpuRearIndex = ActiveIndex = 0;
	 	ElapsedTime = 0.0f;
	 }
}

void ParticleSystem::Render()
{
	if (ActiveIndex == CpuRearIndex) // Nothing To Render
		return ;
	
	WorldTF->BindToGPU();
	Context::Get()->GetViewProjectionCBuffer()->BindToGPU();
	ParticleTexture->BindToGPU(Texture_Slot_PS_Particle);
	ParticleDescCBuffer_VS->BindToGPU();
	ParticleDescCBuffer_GS->BindToGPU();
	CircularVBuffer->BindToGPU();

	if (GpuRearIndex == ActiveIndex)
		return ;

	UINT DrawCount;
	if (GpuRearIndex > ActiveIndex)
	{
		DrawCount = GpuRearIndex - ActiveIndex;
		ParticleShader->Draw(DrawCount, ActiveIndex);
	}
	else
	{
		DrawCount = Data.MaxParticle - ActiveIndex;
		ParticleShader->Draw(DrawCount, ActiveIndex);
		DrawCount = GpuRearIndex;
		ParticleShader->Draw(DrawCount, 0);
	}
}

void ParticleSystem::SetScale(float InScale)
{
	WorldTF->SetScale(InScale * Vector::One);
	ParticleSetting.StartSize = Vector2D(Data.MinStartSize, Data.MaxStartSize) * InScale;
	ParticleSetting.EndSize = Vector2D(Data.MinEndSize, Data.MaxEndSize) * InScale;
}

void ParticleSystem::Reset()
{
	ElapsedTime = 0.0f;
	LastAddTime = sdt::SystemTimer::Get()->GetRunningTime();

	CpuRearIndex = GpuRearIndex = ActiveIndex = 0;

	Vertices.clear();
	Vertices.resize(Data.MaxParticle);
	CircularVBuffer->UpdateBuffer(Vertices.data(), 0, 0);
}

// 특정 위치에 Particle을 생성. 정확히는 Particle System의 한 Texture의 점을 지정해줌.
void ParticleSystem::Add(const Vector& InPosition)
{
	const double CurrentRunningTime = sdt::SystemTimer::Get()->GetRunningTime();
	// constexpr float FPS = 1.f;
	constexpr float FPS = 10.f;
	if (CurrentRunningTime - LastAddTime < 1.f / FPS ) // FPS 제한
		return;

	LastAddTime = static_cast<float>(CurrentRunningTime);

	if (IsBufferFull() == true)
		return;

	CreateSingleParticleVertex(Data, InPosition, ElapsedTime, Vertices[CpuRearIndex]);
	CpuRearIndex = (CpuRearIndex + 1) % Data.MaxParticle;
}

bool ParticleSystem::IsBufferFull() const
{
	UINT NextLeadCount = CpuRearIndex + 1;

	if (NextLeadCount >= Data.MaxParticle)
	{
		NextLeadCount = 0;
		if (Data.bLoop == false && NextLeadCount == ActiveIndex)
			return true;
	}
	return false;
}


void ParticleSystem::SetupParticle(const wstring & FullParticlePath)
{
	ReadFile(FullParticlePath);
	SetTexture(Data.TextureFileName);
	
	ParticleSetting.MinColor = Data.MinColor;
	ParticleSetting.MaxColor = Data.MaxColor;
	ParticleSetting.ColorAmount = Data.ColorAmount;
	ParticleSetting.Gravity = Data.Gravity;
	ParticleSetting.RotateSpeed = Vector2D(Data.MinRotateSpeed, Data.MaxRotateSpeed);
	ParticleSetting.StartSize = Vector2D(Data.MinStartSize, Data.MaxStartSize);
	ParticleSetting.EndSize = Vector2D(Data.MinEndSize, Data.MaxEndSize);
	ParticleSetting.Duration = Data.Duration;
	ParticleSetting.DurationModifier = Data.DurationModifier;
}

void ParticleSystem::SetTexture(const wstring& InFileName)
{
	SAFE_DELETE(ParticleTexture);
	
	ParticleTexture = new Texture(L"Particles/" + InFileName, true);
}

void ParticleSystem::UpdateParticleVertices()
{
	if (GpuRearIndex == CpuRearIndex)
		return;
	const int UpdateCount = (Data.MaxParticle + CpuRearIndex - GpuRearIndex) % Data.MaxParticle;
	CircularVBuffer->UpdateBuffer(Vertices.data(), GpuRearIndex, UpdateCount);
	GpuRearIndex = CpuRearIndex;
}

void ParticleSystem::CheckExpiredParticle() 
{
	while (ActiveIndex != GpuRearIndex)
	{
		const float Age = ElapsedTime - Vertices[ActiveIndex].SpawnedTime;

		if (Age <= Data.Duration) // Alive
			return;
		// Expired
		ActiveIndex++;

		if (ActiveIndex >= Data.MaxParticle)
			ActiveIndex = Data.bLoop ? 0 : Data.MaxParticle;
	}
}
