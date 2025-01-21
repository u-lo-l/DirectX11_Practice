#include "framework.h"
#include "ConstantDataBinder.h"
UINT ConstantDataBinder::Count = 0;

ConstantDataBinder::ConstantDataBinder( Shader * InDrawer )
 : Drawer( InDrawer )
{
	string DataInfo = "World Context Desc #" + to_string( ConstantDataBinder::Count );
	ContextBuffer = new ConstantBuffer(&ContextDescData, DataInfo, sizeof(ContextDesc));
	ContextECB = Drawer->AsConstantBuffer("CB_Context");
	ConstantDataBinder::Count++;
}

ConstantDataBinder::~ConstantDataBinder()
{
	ConstantDataBinder::Count--;
	SAFE_DELETE( ContextBuffer );
}

void ConstantDataBinder::Tick()
{
	ContextDescData.View = Context::Get()->GetViewMatrix();
	ContextDescData.ViewInv = Matrix::Invert(ContextDescData.View);
	ContextDescData.Projection = Context::Get()->GetProjectionMatrix();
	ContextDescData.ViewProjection = ContextDescData.View * ContextDescData.Projection;
	ContextDescData.LightDirection = Context::Get()->GetLightDirection();
}

void ConstantDataBinder::BindToGPU() const
{
	ContextBuffer->BindToGPU();
	CHECK(ContextECB->SetConstantBuffer(*ContextBuffer) >= 0);
}
