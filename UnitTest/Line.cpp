#include "Pch.h"
#include "Line.h"

void Line::Initialize()
{
	MessageBox(nullptr, L"Initialize", L"Message", MB_OK);
}

void Line::Destroy()
{
	MessageBox(D3D::GetDesc().Handle, L"Destroy", L"Message", MB_OK);
}

void Line::Tick()
{

}

void Line::Render()
{

}
