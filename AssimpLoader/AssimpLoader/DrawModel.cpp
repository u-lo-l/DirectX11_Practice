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
		MainCamera->SetPosition(0, 150, -250);

		// const vector<wstring> ModelNames = {L"XYBot", L"Shannon", L"Airplane", L"Mousey"};
		const vector<wstring> ModelNames = {L"Shannon"};


		for (const wstring & ModelName : ModelNames)
		{
			Models.push_back(new Model(ModelName));
		}

		// SetBoneIndicators();
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
		static int Pass = 1;
		ImGui::SliderInt("Model", &ModelIndex, 0, Models.size() - 1);
		ImGui::SliderInt("Pass", &Pass, 0, 1);
		ImGui::SliderInt("ShowBone", &ShowBone, 0, 1);

		if (prevIndex != ModelIndex)
			SetBoneIndicators();
		Models[ModelIndex]->Tick();
		Models[ModelIndex]->SetPass(Pass);
	}

	void DrawModel::Render()
	{
		Models[ModelIndex]->Render();

		if (ShowBone == false)
			return ;
		for (const Vector& v : Positions)
		{
			Sphere->SetWorldPosition(v);
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

		const UINT BoneCount = model->GetBoneCount(); 
		for (UINT i = 0 ; i < BoneCount ; i += 1)
		{
			Matrix Mat = model->GetBoneTransforms()[i];
			
			Vector v = Vector::Zero;
			v = Vector::TransformCoord(v, Matrix::Invert(Mat));
		
			Positions.push_back(v);
		}
		
		Sphere = new Model(L"Sphere");
		Sphere->GetWorldTransform()->SetScale({0.015f, 0.015f, 0.015f});
	}
}
