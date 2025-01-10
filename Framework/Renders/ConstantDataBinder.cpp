#include "framework.h"
#include "ConstantDataBinder.h"

ConstantDataBinder::ConstantDataBinder( Shader * InDrawer )
 : Drawer( InDrawer )
{
	ContextBuffer = new ConstantBuffer(&ContextDescData, "Context.Matrix.Constants", sizeof(ContextDesc));
	ContextECB = Drawer->AsConstantBuffer("CB_Context");
}

ConstantDataBinder::~ConstantDataBinder()
{
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
