#include "Pch.h"
#include "InstancingDemo.h"

namespace sdt
{
	void InstancingDemo::Initialize()
	{
		Camera * const MainCamera = Context::Get()->GetCamera();
		 
		MainCamera->SetPosition( 0, 10, 0 );
		MainCamera->SetRotation( 210, 135, 180);
		SetPlane();
		
		ModelInstances.insert(ModelInstances.end(), {
			{new Model(L"Cube"),{0.2f,0.2f,0.2f}, 10.f},
			{new Model(L"Airplane"),{0.01f,0.01f,0.01f}, 2.f},
			{new Model(L"Sphere"),{0.2f,0.2f,0.2f}, 10.f},
			// {new Model(L"Kachujin"),{0.2f,0.2f,0.2f}, 15.f},
			{new Model(L"Adam"),{0.2f,0.2f,0.2f}, 15.f},
		});
		
		constexpr UINT InstanceCount = 169;
		constexpr float Stride = 40.f;
		SetModelsPosition(InstanceCount, Stride);
	}

	void InstancingDemo::Destroy()
	{
		SAFE_DELETE(Plane);
		for (ModelInstanceData ModelAndScale : ModelInstances)
		{
			SAFE_DELETE(ModelAndScale.Object);
		}
	}

	void InstancingDemo::Tick()
	{
		if (!!Plane)
			Plane->Tick();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Tick();
	}

	void InstancingDemo::Render()
	{
		if (!!Plane)
			Plane->Render();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Render();
	}

	void InstancingDemo::SetPlane()
	{
		Plane = new Model(L"Plane");
		Transform * tf = Plane->AddTransforms();
		tf->SetPosition({0,-10,0});
		tf->SetScale({5,1,5});
		tf->SetRotation({0,0,90});
		tf->UpdateWorldMatrix();;
	}

	void InstancingDemo::SetModelsPosition( int MaxInstanceCount, float Stride )
	{
		const int Width = static_cast<int>(sqrt(MaxInstanceCount));
		const int Height = (MaxInstanceCount + Width - 1) / Width;
		const int Depth = ModelInstances.size();
		
		for (int ModelIndex = 0 ; ModelIndex < Depth ; ModelIndex++)
		{
			Vector InstancePosition;
			InstancePosition.Y = static_cast<float>(ModelIndex) * Stride;
			for (int H = 0 ; H < Height ; H++)
			{
				InstancePosition.Z = static_cast<float>(H) * Stride - static_cast<float>(Height) * Stride / 2;
				for (int W  = 0 ; W < Width ; W++)
				{
					const int InstanceIndex = H * Width + W;
					if (InstanceIndex >= MaxInstanceCount)
						break;
					InstancePosition.X = static_cast<float>(W) * Stride - static_cast<float>(Width) * Stride / 2;
					Transform * TF = ModelInstances[ModelIndex].Object->AddTransforms();
					TF->SetScale(ModelInstances[ModelIndex].Scale);
					TF->SetPosition(InstancePosition);
					TF->SetRotation({0, 0, Math::Random(-180.f, 180.f)});
					TF->UpdateWorldMatrix();

					const int AnimationNum = ModelInstances[ModelIndex].Object->GetClipCount();
					if (AnimationNum > 0)
					{
						ModelInstances[ModelIndex].Object->SetClipIndex(InstanceIndex, Math::Random(0, AnimationNum));
					}
				}
			}
		}
	}
}
