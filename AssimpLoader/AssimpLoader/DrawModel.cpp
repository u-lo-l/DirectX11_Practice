#include "Pch.h"
#include "DrawModel.h"

namespace Sdt
{
	DrawModel::~DrawModel()
	{
		for (const Model * Model : Models)
			SAFE_DELETE(Model);
	}

	void DrawModel::Initialize()
	{
		Camera * MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition(0, 200, -300);
		MainCamera->SetRotation(0.1f, 0, 0);
		
		vector<wstring> ModelNames = {L"Mousey"};

		for (const wstring & ModelName : ModelNames)
		{
			Models.push_back(new Model(ModelName));
		}
	}

	void DrawModel::Destroy()
	{
	}

	void DrawModel::Tick()
	{
		//ImGui::SliderInt("Model", &ModelIndex, 0, Models.size() - 1);
	
		 Models[ModelIndex]->Tick();
	}

	void DrawModel::Render()
	{
		 Models[ModelIndex]->Render();
	}
}
