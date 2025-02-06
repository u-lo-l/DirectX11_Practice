#include "Pch.h"
#include "InstancingDemo.h"

namespace Sdt
{
	void InstancingDemo::Initialize()
	{
		Context::Get()->GetCamera()->SetPosition(0, 50, -250);
		SetPlane();
		
		ModelInstances.insert(ModelInstances.end(), {
			{new Model(L"Cube"),{0.075f,0.075f,0.075f}, 10.f},
			{new Model(L"Airplane"),{0.0025f,0.0025f,0.0025f}, 2.f},
			{new Model(L"Adam"),{0.15f,0.15f,0.15f}, 15.f},
			{new Model(L"Kachujin"),{0.1f,0.1f,0.1f}, 15.f},
			{new Model(L"Sphere"),{0.075f,0.075f,0.075f}, 10.f},
		});
		
		SetModelsPosition(16, 35.f);
	}

	void InstancingDemo::Destroy()
	{
		SAFE_DELETE(Plane);
		SAFE_DELETE(Drawer);
		for (ModelInstanceData ModelAndScale : ModelInstances)
		{
			SAFE_DELETE(ModelAndScale.Object);
		}
	}

	void InstancingDemo::Tick()
	{
		Plane->Tick();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Tick();
	}

	void InstancingDemo::Render()
	{
		Plane->Render();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Render();
	}

	void InstancingDemo::SetPlane()
	{
		Plane = new Model(L"Plane");
		Transform * tf = Plane->AddTransforms();
		tf->SetPosition({0,-10,0});
		tf->SetScale({10,1,10});
		tf->SetRotation({0,0,90});
		tf->UpdateWorldMatrix();;
	}

	void InstancingDemo::SetModelsPosition( int MaxInstanceCount, float Stride )
	{
		Transform * tf = nullptr;
		int Width = sqrt(MaxInstanceCount);
		int Height = (MaxInstanceCount + Width - 1) / Width;
		const int Depth = ModelInstances.size();
		
		for (int ModelIndex = 0 ; ModelIndex < Depth ; ModelIndex++)
		{
			Vector InstancePosition;
			InstancePosition.Y = static_cast<float>(ModelIndex) * Stride;
			for (int i = 0 ; i < Height ; i++)
			{
				InstancePosition.Z = static_cast<float>(i) * Stride - static_cast<float>(Height) * Stride / 2;
				for (int j = 0 ; j < Width ; j++)
				{
					const int InstanceIndex = i * Width + j;
					if (InstanceIndex >= MaxInstanceCount)
						break;
					InstancePosition.X = static_cast<float>(j) * Stride - static_cast<float>(Width) * Stride / 2;
					tf = ModelInstances[ModelIndex].Object->AddTransforms();
					tf->SetScale(ModelInstances[ModelIndex].Scale);
					tf->SetPosition(InstancePosition);
					tf->SetRotation({0, 0, Math::Random(-180.f, 180.f)});
					tf->UpdateWorldMatrix();

					int AnimationNum = ModelInstances[ModelIndex].Object->GetClipCount();
					if (AnimationNum > 0)
					{
						ModelInstances[ModelIndex].Object->SetClipIndex(InstanceIndex, Math::Random(0, AnimationNum));
					}
				}
			}
		}
	}
}
