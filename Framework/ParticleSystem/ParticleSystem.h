#pragma once
#include "ParticleData.h"

/**
 * @property ParticleData Data : 파티틀.config로 부터 얻은 데이터 
 * @property ParticleDesc ParticleSetting : GPU에 넘기기 위한 구조체
 *
 * 
 */
class ParticleSystem
{
	static constexpr int Const_Slot_VS_VieProjection   = 1;
	static constexpr int Const_Slot_PS_LightData	   = 2;
	static constexpr int Const_Slot_VS_ParticleSetting = 3;
	static constexpr int Const_Slot_GS_ParticleSetting = 4;
	static constexpr int Texture_Slot_PS_Particle	   = 0;
	static constexpr int Sampler_Slot_PS_Linear		   = 0;
	
	const static wstring ShaderName;
	using VertexType = VertexParticle;
	using ShaderTypeName = HlslShader<VertexType>;
public:
	explicit ParticleSystem(const wstring & InFileName);
	~ParticleSystem();
	void Tick();
	void Render();

	void Reset();
	void Add(const Vector& InPosition);
	void SetScale(float InScale);
	ParticleData & GetParticleData() { return Data; }
	
private:
	bool IsBufferFull() const;
	
	void SetupParticle(const wstring & FullParticlePath);
	void SetTexture(const wstring& InFileName);
	
	
	void UpdateParticleVertices();
	void CheckExpiredParticle();
	
	// ReadFile 과정에서 초기화 되는 구조체
	// 이후 SetTexture시에만 변경됨. GPU에 전달되는 Data아님
	// SetTexture가 생성자 이후에도 호출되지 않으면 Data는 ConstStruct임
	ParticleData Data {};

	Transform * WorldTF = nullptr;
	Texture * ParticleTexture = nullptr;
	ShaderTypeName * ParticleShader = nullptr;

	// this->Data의 값을 이 구조체로 복사해서 GPU로 넣어줌.
	// this->Data가 변경되지 않는다면 Setting도 바뀌지 않음. (X) 바뀐다.
	// Setting의 CurrentTime이 Tick에서 바뀜
	// this->Data가 바뀌지 않으므로 ParticleDescCBuffer가 처음 초기화 이후에 변경되지 않는 상수버퍼이다.
	ParticleDesc ParticleSetting {};
	ConstantBuffer * ParticleDescCBuffer_VS = nullptr;
	ConstantBuffer * ParticleDescCBuffer_GS = nullptr;

	vector<VertexType> Vertices;
	CircularVertexBuffer * CircularVBuffer = nullptr;

	float ElapsedTime = 0.0f;
	float LastAddTime = 0.0f;

	UINT CpuRearIndex = 0; // CPU에서 관리중인 Particle 수
	UINT GpuRearIndex = 0;	// Gpu에 올라간 Particle 수
	UINT ActiveIndex = 0; // 활성화 된 Particle의 시작 Index
	
#pragma region Read File
public:
	void ReadFile(const wstring& InFileName);
	void ReadColor(const Json::Value& Value);
	void ReadSettings(const Json::Value& Value);
	void ReadSize(const Json::Value& Value);
	void ReadSpeed(const Json::Value& Value);
	void ReadVelocity(const Json::Value& Value);
private:
	static void CreateSingleParticleVertex
	(
		const ParticleData & InData,
		const Vector& InPosition,
		float InElapsedTime,
		VertexType & OutParticleVertex
	);
#pragma endregion Read File
};




