#include "framework.h"
#include "RenderManager.h"

class Material;

MaterialBatch::MaterialBatch(const Material* InMaterial)
	: Mat(InMaterial)
{
}

void MaterialBatch::AddRenderable(const ARenderable* InRenderable)
{
	Elements.push_back(InRenderable);
}

void MaterialBatch::Tick()
{
	for ( const ARenderable * Elem : Elements)
	{
		Elem->Tick();
	}
}

void MaterialBatch::Render(const RenderingShader* InShader) const
{
	ASSERT(!!Mat, "Material Not Valid")
	Mat->BindToGpu(1);
	for ( const ARenderable * Elem : Elements)
	{
		Elem->Render(InShader);
	}
}

ShaderBatch::ShaderBatch(const Material* InMaterial)
{
	ASSERT(!!InMaterial, "Material Not Valid");
	Shader = ShaderManager::Get()->GetRenderShader(InMaterial->GetShaderName());
	ASSERT(!!Shader, "Shader Not Valid");
}

void ShaderBatch::AddRenderable(const ARenderable* InRenderable)
{
	const Material * Mat = InRenderable->GetMaterial();
	
	const auto & Cit = BatchIndices.find(Mat);
	int BatchIndex;
	if (Cit == BatchIndices.cend())
	{
		BatchIndex = BatchIndices.size();
		BatchIndices[Mat] = BatchIndex;
		Batches.push_back(new MaterialBatch(Mat));
	}
	else
	{
		BatchIndex = Cit->second;
	}
	Batches[BatchIndex]->AddRenderable(InRenderable);
}

void ShaderBatch::Tick()
{
	for (MaterialBatch * MatBatch : Batches)
	{
		MatBatch->Tick();
	}
}

void ShaderBatch::Render() const
{
	Shader->SetPass(0);
	// TODO : Bind Resources Per Shader
	for (const MaterialBatch * MatBatch : Batches)
	{
		MatBatch->Render(Shader);
	}
	RenderingShader::ClearPass();
}

RenderManager * RenderManager::Instance = nullptr;

void RenderManager::Create()
{
	ASSERT(Instance == nullptr, "Instance Already Exist");
	Instance = new RenderManager();
}

void RenderManager::Destroy()
{
	SAFE_DELETE(Instance);
}

RenderManager * RenderManager::Get()
{
	ASSERT(Instance != nullptr, "Instance Not Exist");
	return Instance;
}

void RenderManager::AddRenderable(const ARenderable* InRenderable)
{
	const Material * Mat = InRenderable->GetMaterial();
	const RenderingShader * Shader = InRenderable->GetShader();
	
	const auto & Cit = QueueIndices.find(Shader);
	int BatchIndex;
	if (Cit == QueueIndices.cend())
	{
		BatchIndex = QueueIndices.size();
		QueueIndices[Shader] = BatchIndex;
		RenderQueue.push_back(new ShaderBatch(Mat));
		BatchCount++;
	}
	else
	{
		BatchIndex = Cit->second;
	}
	RenderQueue[BatchIndex]->AddRenderable(InRenderable);
	RenderableCount++;
}

void RenderManager::Tick()
{
#if defined(DISPLAY_DEBUG)
	ImGui::Begin("Render Manager");
	ImGui::Text("Render Queue Size : %d", GetRenderQueueSize());
	ImGui::Text("Renderables Count : %d", RenderableCount);
	int QueueIndex = 0;
	for (const ShaderBatch * S : RenderQueue)
	{
		int BatchIndex = 0;
		ImGui::Text("Render Queue #%d", ++QueueIndex);
		for (const MaterialBatch * M : S->GetBatches())
		{
			int ElementIndex = 0;
			ImGui::Text("  Batch #%d : %s", ++BatchIndex, M->GetMaterial()->GetMaterialName().c_str());
			for (const ARenderable * R : M->GetElements())
			{
				ImGui::Text("    Element #%d : %s", ++ElementIndex, R->GetName().c_str());
			}
		}
	}
	ImGui::End();
#endif
	if (!!CB_PerFrame)
	{
		CB_PerFrameData = {
			Context::Get()->GetCamera()->GetViewMatrix(),
			Context::Get()->GetCamera()->GetProjectionMatrix(),
			Context::Get()->GetCamera()->GetViewProjectionMatrix(),
			Context::Get()->GetCamera()->GetPosition(),
			0,
			Context::Get()->GetLightColor(),
			Context::Get()->GetLightDirection(),
			0
		};
		CB_PerFrame->UpdateData(&CB_PerFrameData, sizeof(CB_PerFrameDesc));
	}

	for (ShaderBatch * Batch : RenderQueue)
	{
		Batch->Tick();
	}
}

void RenderManager::Render() const
{
	if (!!CB_PerFrame)
	{
		CB_PerFrame->BindToGPU(ShaderType::VP, 0);
	}
	for (const ShaderBatch * Batch : RenderQueue)
	{
		Batch->Render();
	}
}

RenderManager::RenderManager()
{
	CB_PerFrame = new ConstantBuffer (
		ShaderType::VP,
		0,
		&CB_PerFrameData,
		sizeof(CB_PerFrameDesc),
		false
	);
}

RenderManager::~RenderManager()
{
	SAFE_DELETE(CB_PerFrame);
}
