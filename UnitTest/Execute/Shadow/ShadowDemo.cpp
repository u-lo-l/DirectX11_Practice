#include "Pch.h"
#include "ShadowDemo.h"

namespace sdt
{
	void ShadowDemo::Initialize()
	{
		Shadow * const ShadowMap = Context::Get()->GetShadowMap();
		DepthDrawer = new Hlsl2DTextureShader(*ShadowMap->GetRenderTarget());

		float TextureWidth = 0.1f * ShadowMap->GetWidth();
		float TextureHeight = 0.1f * ShadowMap->GetHeight();
		
		DepthDrawer->GetTransform()->SetScale({TextureWidth, TextureHeight, 1});
		DepthDrawer->GetTransform()->SetWorldPosition({TextureWidth,TextureHeight, 0});

		Camera * const MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition( 150, 150, 20 );
		MainCamera->SetRotation( 225, 0, 180);

		LoadSky();
		LoadModel();
	}

	void ShadowDemo::Destroy()
	{
		SAFE_DELETE(Plane);
		for (ModelInstanceData ModelAndScale : ModelInstances)
		{
			SAFE_DELETE(ModelAndScale.Object);
		}
		for (ModelInstanceData ModelAndScale : ModelInstances_ForLighting)
		{
			SAFE_DELETE(ModelAndScale.Object);
		}
		SAFE_DELETE(Grasses);
		SAFE_DELETE(Sky);
		SAFE_DELETE(Rain);
		SAFE_DELETE(Particle_Fire);
	}

	void ShadowDemo::Tick()
	{
		if (!!Sky)
			Sky->Tick();
		if (!!Plane)
			Plane->Tick();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Tick();
		
		if (!!DepthDrawer)
			DepthDrawer->Tick();
	}

	void ShadowDemo::PreRender()
	{
		// Render
		Shadow * const ShadowMap = Context::Get()->GetShadowMap();
		if (!!ShadowMap)
			ShadowMap->PreRender();
		if (!!Plane)
			Plane->RenderShadow();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->RenderShadow();
	}
	
	void ShadowDemo::Render()
	{
		if (!!Sky)
			Sky->Render();
		Shadow * const ShadowMap = Context::Get()->GetShadowMap();
		if (Keyboard::Get()->IsPressed(VK_SPACE))
		{
			ShadowMap->GetRenderTarget()->SaveTexture(L"Shadow");
		}
		ID3D11ShaderResourceView * ShadowSRV = *ShadowMap->GetRenderTarget();
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(5, 1, &ShadowSRV);
		if (!!Plane)
			Plane->Render();
		for (const ModelInstanceData & P : ModelInstances)
			P.Object->Render();
	}

	void ShadowDemo::PostRender()
	{
		if (!!DepthDrawer) // 원본 축소
		{
			DepthDrawer->SetSRV(*Context::Get()->GetShadowMap()->GetRenderTarget());
			DepthDrawer->Tick();
			DepthDrawer->Render();
		}
	}

	void ShadowDemo::LoadSky()
	{
		Sky = new SkySphere(L"Environments/SunSetCube1024.dds", 0.5f);
	}

	void ShadowDemo::LoadModel()
	{
		constexpr UINT InstanceCount = 4;
		constexpr float Stride = 40.f;
		if (!Plane)
			SetPlane();
		
		ModelInstances.insert(ModelInstances.end(), {
			{new Model(L"Adam"),{0.2f,0.2f,0.2f}, 15.f},
			// {new Model(L"Sphere"),{0.1f,0.1f,0.1f}, 15.f},
		});
		SetModelsPosition_Square(ModelInstances, InstanceCount, Stride);
	}

	void ShadowDemo::SetPlane()
	{
		Plane = new Model(L"Plane");
		Plane->SetTiling({10,10});
		Transform * tf = Plane->AddTransforms();
		tf->SetWorldPosition({0,0,0});
		tf->SetScale({2.5,1,2.5});
		tf->SetWorldRotation({0, Math::PiOver2,0});
	}
	
	void ShadowDemo::SetModelsPosition_Square(const vector<ModelInstanceData> & Models, int MaxInstanceCount, float Stride, const Vector& Center ) const
	{
		const int Width = static_cast<int>(sqrt(MaxInstanceCount));
		const int Height = (MaxInstanceCount + Width - 1) / Width;
		const int Depth = ModelInstances.size();
		Vector Offet = Center - 0.5f * Vector(static_cast<float>(Width - 1) * Stride ,0, static_cast<float>(Height - 1) * Stride);

		for (int ModelIndex = 0 ; ModelIndex < Depth ; ModelIndex++)
		{
			Vector InstancePosition;
			InstancePosition.Y = static_cast<float>(ModelIndex) * Stride;
			for (int H = 0 ; H < Height ; H++)
			{
				InstancePosition.Z = static_cast<float>(H) * Stride;// - static_cast<float>(Height) * Stride / 2;
				for (int W  = 0 ; W < Width ; W++)
				{
					const int InstanceIndex = H * Width + W;
					if (InstanceIndex >= MaxInstanceCount)
						break;
					InstancePosition.X = static_cast<float>(W) * Stride;// - static_cast<float>(Width) * Stride / 2;
					Transform * TF = Models[ModelIndex].Object->AddTransforms();
					TF->SetScale(Models[ModelIndex].Scale);
					TF->SetWorldPosition(Offet + InstancePosition);
					TF->SetWorldRotation({0, Math::Random(-Math::Pi, Math::Pi), 0});

					const int AnimationNum = Models[ModelIndex].Object->GetClipCount();
					if (AnimationNum > 0)
					{
						Models[ModelIndex].Object->SetClipIndex(InstanceIndex, Math::Random(0, AnimationNum));
					}
				}
			}
		}
	}

	void ShadowDemo::SetModelsPosition_Line
	(
		const vector<ModelInstanceData>& Models,
		int MaxInstanceCount,
		float Stride,
		const Vector& Center
	) const
	{
		const int Width = Models.size();
		const int Depth = (MaxInstanceCount);
		Vector Offet = Center - 0.5f * Vector(static_cast<float>(Width - 1) * Stride ,0, static_cast<float>(Depth - 1) * Stride);
		for (int ModelIndex = 0 ; ModelIndex < Width ; ModelIndex++)
		{
			Vector InstancePosition;
			InstancePosition.X = Stride * static_cast<float>(ModelIndex);
			for (int InstanceIndex = 0 ; InstanceIndex < Depth ; InstanceIndex++)
			{
				InstancePosition.Z = Stride * static_cast<float>(InstanceIndex);
				Transform * TF = Models[ModelIndex].Object->AddTransforms();
				TF->SetScale(Models[ModelIndex].Scale);
				TF->SetWorldPosition(Offet + InstancePosition);
				TF->SetWorldRotation({0, Math::Random(-Math::Pi, Math::Pi), 0});

				const int AnimationNum = Models[ModelIndex].Object->GetClipCount();
				if (AnimationNum > 0)
				{
					Models[ModelIndex].Object->SetClipIndex(InstanceIndex, Math::Random(0, AnimationNum));
				}
			}
		}
	}
}
