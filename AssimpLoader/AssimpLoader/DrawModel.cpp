#include "Pch.h"
#include "DrawModel.h"

namespace Sdt
{
	DrawModel::~DrawModel()
	{
		Destroy();
	}

	void DrawModel::Initialize()
	{
		Camera * MainCamera = Context::Get()->GetCamera();
		MainCamera->SetRotation( 211, 133, 180);
		MainCamera->SetPosition( -138, 108, -142 );

		// const vector<wstring> ModelNames = {L"XYBot", L"Shannon", L"Airplane", L"Mousey"};
		const vector<wstring> ModelNames = {L"Adam"};
		// const vector<wstring> ModelNames = {L"Airplane"};
		// const vector<wstring> ModelNames = {L"Mousey", L"Adam", L"Airplane"};

		for (const wstring & ModelName : ModelNames)
		{
			Models.push_back(new Model(ModelName));
		}
	}

	void DrawModel::Destroy()
	{
		for (Model* model : Models)
			SAFE_DELETE(model);
	}

	void DrawModel::Tick()
	{
		ImGui::SliderInt("Model", &ModelIndex, 0, Models.size() - 1);
		Models[ModelIndex]->Tick();
	}

	void DrawModel::Render()
	{
		Models[ModelIndex]->Render();
	}
}
