#include "Pch.h"
#include "DrawModel.h"

namespace sdt
{
	DrawModel::~DrawModel()
	{
		Destroy();
	}

	void DrawModel::Initialize()
	{
		Camera * MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 0, 10, -50 );

		// const vector<wstring> ModelNames = {L"XYBot", L"Shannon", L"Airplane", L"Mousey"};
		// const vector<wstring> ModelNames = {L"Kachujin", L"Mousey", L"Adam"};
		const vector<wstring> ModelNames = {L"Adam"};
		// const vector<wstring> ModelNames = {L"Mousey", L"Adam", L"Airplane"};

		for (const wstring & ModelName : ModelNames)
		{
			Model * M = new Model(ModelName);
			Transform * tf = M->AddTransforms();
			tf->SetWorldPosition({0,0,0});
			tf->SetScale({0.1f,0.1f,0.1f});
			Models.push_back(M);
		}
	}

	void DrawModel::Destroy()
	{
		for (Model* model : Models)
			SAFE_DELETE(model);
	}

	void DrawModel::Tick()
	{
		if (Models.empty() == true)
			return ;
		
		const float DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
		Vector DeltaPosition = {0, 0, 0};

		if (Mouse::Get()->IsPress(MouseButton::Left) == true)
		{
			if (Keyboard::Get()->IsPressed('W') == true)
			{
				DeltaPosition += Vector::Forward * DeltaTime * 20;
			}
			if (Keyboard::Get()->IsPressed('S') == true)
			{
				DeltaPosition -= Vector::Forward * DeltaTime * 20;
			}
			if (Keyboard::Get()->IsPressed('D') == true)
			{
				DeltaPosition += Vector::Right * DeltaTime * 20;
			}
			if (Keyboard::Get()->IsPressed('A') == true)
			{
				DeltaPosition -= Vector::Right * DeltaTime * 20;
			}
			Models[ModelIndex]->GetTransform(0)->AddLocalTranslation(DeltaPosition);
		}
		ImGui::SliderInt("Model", &ModelIndex, 0, Models.size() - 1);
		Models[ModelIndex]->Tick();
	}

	void DrawModel::Render()
	{
		Models[ModelIndex]->Render();
	}
}
