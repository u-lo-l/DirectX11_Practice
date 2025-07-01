#pragma once
#include "Renders/Renderable/ARenderable.h"

class ARenderable;

class MaterialBatch
{
public :
	explicit MaterialBatch(const Material * InMaterial);
	void AddRenderable(const ARenderable * InRenderable);
	void Tick();
	void Render(const RenderingShader* InShader, int& DrawCallCount) const;
	const Material * GetMaterial() const { return Mat; }
	size_t GetRenderableCount() const { return Elements.size(); }
	const vector<const ARenderable *> & GetElements() const { return Elements; }
	const Material * Mat = nullptr;
	vector<const ARenderable *> Elements;
};

/**
 * @brief
 * MeshBatch :
 * -Material
 * -Shader Pipeline (Shader Class) 
 * -InputLayout
 * -Pass (Opaque / Transparent / ... )
 * -Primitive (TriangleList / LineList / ... )
 * 
 * 가 동일한 Renderable. Material이 같으면 저거 다 같음
 */
class ShaderBatch
{
public:
	explicit ShaderBatch(const Material * InMaterial);
	void AddRenderable(const ARenderable * InRenderable);
	void Tick();
	void Render(int& DrawCallCount) const;
	size_t GetMaterialBatchCount() const { return Batches.size(); }
	const vector<MaterialBatch *> & GetBatches() const { return Batches; }
private:
	RenderingShader * Shader = nullptr;
	map<const Material *, int> BatchIndices;
	vector<MaterialBatch *> Batches;
};

class RenderManager
{
public:
	static void Create();
	static void Destroy();
	static RenderManager * Get();
	void SetViewPort(float InWidth, float InHeight, float InX = 0, float InY = 0, float InMinDepth = 0, float InMaxDepth = 1);
	void AddRenderable(const ARenderable * InRenderable);
	void Tick();
	void Render();
	size_t GetRenderQueueSize() const { return RenderQueue.size(); }
	const vector<ShaderBatch *> & GetRenderQueue() const { return RenderQueue; }
	void ImGuiDebugMessage() const;
private:
	RenderManager();
	~RenderManager();
	static RenderManager * Instance;

private:
	map<const RenderingShader * , int> QueueIndices;
	vector<ShaderBatch *> RenderQueue;
	int BatchCount = 0;
	int RenderableCount = 0;

	struct CB_PerFrameDesc
	{
		Matrix View;
		Matrix Projection;
		Vector CameraPosition;
		float Padding1;
		Color LightColor;
		Vector LightDirection;
		float Padding2;
	} CB_PerFrameData ;
	
	ConstantBuffer * CB_PerFrame = nullptr;
	StructuredBuffer * SB_PerFrame = nullptr;
	ViewPort * Vp;
	
	int DrawCallCount = 0;
};
