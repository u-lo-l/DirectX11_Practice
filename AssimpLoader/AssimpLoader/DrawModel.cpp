#include "Pch.h"
#include "DrawModel.h"
#include "Converter.h"

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
		MainCamera->SetPosition(150, 100, -300);
		MainCamera->SetRotation(0, 0, 0);
		
		Model * Temp = nullptr;

		Temp = new Model();
		Temp->ReadMaterial(L"Shannon/Shannon");
		Temp->ReadMesh(L"Shannon/Shannon");
		Models.push_back(Temp);

		Temp = new Model();
		Temp->ReadMaterial(L"Cube/Cube");
		Temp->ReadMesh(L"Cube/Cube");
		Models.push_back(Temp);
	}

	void DrawModel::Destroy()
	{
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
