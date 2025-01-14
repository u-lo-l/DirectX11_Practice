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
		MainCamera->SetPosition(0, 150, -250);

		const vector<wstring> ModelNames = {L"YBot", L"Shannon", L"Manny", L"Airplane", L"Mousey"};

		for (const wstring & ModelName : ModelNames)
		{
			Models.push_back(new Model(ModelName));
		}

		SetBoneIndicators();
	}

	void DrawModel::Destroy()
	{
		SAFE_DELETE(Sphere);
		
		for (Model* model : Models)
			SAFE_DELETE(model);
	}

	void DrawModel::Tick()
	{
		int prevIndex= ModelIndex;
		ImGui::SliderInt("Model", &ModelIndex, 0, Models.size() - 1);
		if (prevIndex != ModelIndex)
		{
			SetBoneIndicators();
		}
		Models[ModelIndex]->Tick();
	}

	void DrawModel::Render()
	{
		static int Pass = 1;
		ImGui::SliderInt("Pass", &Pass, 0, 1);

		Models[ModelIndex]->SetPass(Pass);
		Models[ModelIndex]->Render();

		for (const Vector& v : Positions)
		{
			Sphere->GetWorldTransform()->SetPosition(v);

			Sphere->Tick();
			Sphere->Render();
		}
	}

	void DrawModel::SetBoneIndicators()
	{
		SAFE_DELETE(Sphere);
		Positions.clear();
		
		Model *	model = Models[ModelIndex];
		Matrix WorldMat = model->GetWorldTransform()->GetMatrix(); 		

		const UINT BoundCount = model->GetBoneCount(); 
		for (UINT i = 0 ; i < BoundCount ; i += 1)
		{
			Matrix Mat = model->GetBoneTransforms()[i];
			
			Vector v = Vector::Zero;
			v = Vector::TransformCoord(v, Mat);
		
			Positions.push_back(v);
		}
		
		Sphere = new Model(L"Sphere");
		Sphere->GetWorldTransform()->SetScale({0.02f, 0.02f, 0.02f});
	}
}
