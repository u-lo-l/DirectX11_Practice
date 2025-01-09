#include "Pch.h"
#include "DrawModel.h"
#include "Converter.h"

namespace Sdt
{

	void DrawModel::Initialize()
	{
		Camera * MainCamera = Context::Get()->GetCamera();
		MainCamera->SetPosition(150, 100, -300);
		MainCamera->SetRotation(0, 0, 0);
		Drawer = new Shader(L"19_ModelVertex.fx");
		
		Cube = new Model();
		Cube->ReadMesh(L"cubb/cubb");
		Cube->BindData(Drawer);
	}

	void DrawModel::Destroy()
	{
	}

	void DrawModel::Tick()
	{
		Cube->Tick();
	}

	void DrawModel::Render()
	{
		Cube->Render();
	}
}
