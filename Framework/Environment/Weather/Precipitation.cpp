#include "framework.h"
#include "Precipitation.h"

Precipitation::Precipitation(const Vector& InExtent, UINT InDrawCount, const wstring& InFilePath, WeatherType InWeatherType)
{
	ShaderName = L"Weather/41_Weather.hlsl";
	
	UINT ShaderTypeFlag = 0;
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::VertexShader);
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::PixelShader);
	ShaderTypeFlag |= static_cast<UINT>(::ShaderType::GeometryShader);
	WeatherShader = new ShaderType(ShaderName, ShaderTypeFlag);
	WeatherShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	CHECK(WeatherShader->CreateSamplerState_Linear() >= 0);
	CHECK(WeatherShader->CreateRasterizerState_Solid_NoCull() >= 0);
	CHECK(WeatherShader->CreateBlendState_AlphaBlendCoverage() >= 0);

	World = new Transform();
	Image = new Texture(InFilePath, true);
	Type = InWeatherType;
	DrawCount = InDrawCount;

	CreateWeatherData(InExtent);
	CreateWeatherVertexBuffer();
	CreateWeatherConstantBuffers();
}

Precipitation::~Precipitation()
{
	SAFE_DELETE(WeatherShader);
	SAFE_DELETE(World);
	SAFE_DELETE(WeatherSettingCBuffer_VS);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(WeatherSettingCBuffer_GS);
	SAFE_DELETE(ViewProjectionBuffer_GS);
	SAFE_DELETE(WeatherSettingCBuffer_PS);
	SAFE_DELETE(Image);
}

void Precipitation::Tick()
{
	World->SetWorldPosition(Context::Get()->GetCamera()->GetPosition());
	World->Tick();

	ViewProjectionData.View = Context::Get()->GetViewMatrix();
	ViewProjectionData.ViewInv = Matrix::Invert(ViewProjectionData.View);
	ViewProjectionData.Projection = Context::Get()->GetProjectionMatrix();

	VS_WeatherData.Time += sdt::SystemTimer::Get()->GetDeltaTime();
	
	ImGui::SliderFloat3("Origin", VS_WeatherData.Origin, 0, 200);
	ImGui::SliderFloat3("Velocity", VS_WeatherData.Velocity, -200, 200);
	GS_WeatherData.Velocity = VS_WeatherData.Velocity;
	ImGui::ColorEdit3("Color", PS_WeatherData.PrecipitationColor);
	ImGui::SliderFloat("Distance", &GS_WeatherData.DrawDistance, 0, VS_WeatherData.Extent.Z * 2.0f);
	
	WeatherSettingCBuffer_VS->UpdateData(&VS_WeatherData, sizeof(VS_WeatherData));
	WeatherSettingCBuffer_GS->UpdateData(&GS_WeatherData, sizeof(GS_WeatherData));
	WeatherSettingCBuffer_PS->UpdateData(&PS_WeatherData, sizeof(PS_WeatherData));
	ViewProjectionBuffer_GS->UpdateData(&ViewProjectionData, sizeof(ViewProjectionData));
}

void Precipitation::Render()
{
	World->BindToGPU();
	Image->BindToGPU(Texture_Slot_PS_Diffuse);

	WeatherSettingCBuffer_VS->BindToGPU();
	WeatherSettingCBuffer_GS->BindToGPU();
	WeatherSettingCBuffer_PS->BindToGPU();
	ViewProjectionBuffer_GS->BindToGPU();

	VBuffer->BindToGPU();
	
	WeatherShader->Draw(DrawCount);
}

void Precipitation::CreateWeatherData(const Vector & InExtent)
{
	switch (Type)
	{
	case Rain:
		VS_WeatherData = {
		{-10, -100, 0}, 0,
		Vector::Zero,
		1.f,
		InExtent,
		0
	};
		GS_WeatherData = {{-10, -100, 0}, InExtent.Z * 2.f};
		PS_WeatherData = { Color::White };
		break;
	case Snow:
		VS_WeatherData = {
			{0, -5, 0}, 0,
			Vector::Zero,
			5.0f,
			InExtent,
			0
		};
		GS_WeatherData = {{0, -5, 0}, InExtent.Z * 2.f};
		PS_WeatherData = { Color::White };
		break;
	default:
		break;
	}
}

void Precipitation::CreateWeatherVertexBuffer()
{
	vector<VertexType> Vertices = vector<VertexType>(DrawCount);
	for (UINT i = 0; i < DrawCount; i++)
	{
		Vector2D scale;
		scale.X = Math::Random(0.1f, 0.4f);
		scale.Y = Math::Random(2.0f, 6.0f);

		Vector position;
		position.X = Math::Random(-VS_WeatherData.Extent.X, +VS_WeatherData.Extent.X);
		position.Y = Math::Random(-VS_WeatherData.Extent.Y, +VS_WeatherData.Extent.Y);
		position.Z = Math::Random(-VS_WeatherData.Extent.Z, +VS_WeatherData.Extent.Z);

		Vertices[i].Position = position;
		Vertices[i].Scale = scale;

		Vertices[i].Random = 1;
	}
	VBuffer = new VertexBuffer(Vertices.data(), DrawCount, sizeof(VertexType));
}

void Precipitation::CreateWeatherConstantBuffers()
{
	WeatherSettingCBuffer_VS = new ConstantBuffer(
		::ShaderType::VertexShader,
		Const_Slot_VS_WeatherData,
		nullptr,
		"WeatherSetting_VS",
		sizeof(WeatherData_VS),
		false
	);
	WeatherSettingCBuffer_GS = new ConstantBuffer(
		::ShaderType::GeometryShader,
		Const_Slot_GS_WeatherData,
		nullptr,
		"WeatherSetting_GS",
		sizeof(WeatherData_GS),
		false
	);
	WeatherSettingCBuffer_PS = new ConstantBuffer(
		::ShaderType::PixelShader,
		Const_Slot_PS_WeatherData,
		nullptr,
		"WeatherSetting_PS",
		sizeof(WeatherData_PS),
		false
	);
	ViewProjectionBuffer_GS = new ConstantBuffer(
		::ShaderType::GeometryShader,
		Const_Slot_GS_VieProjection,
		nullptr,
		"ViewProjection_GS",
		sizeof(ViewProjectionDesc),
		false
	);
}

