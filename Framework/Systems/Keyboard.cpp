#include "Framework.h"
#include "Keyboard.h"
#include <winuser.h> // for GetAsyncKeyState()

Keyboard * Keyboard::Instance = nullptr;

void Keyboard::Create()
{
	ASSERT(static_cast<int>(Instance == nullptr), "Keyboard Instance must null on Create()");
	Instance = new Keyboard();

}

void Keyboard::Destroy()
{
	ASSERT(static_cast<int>(Instance != nullptr), "Keyboard Instance must not null on Destroy()");
	SAFE_DELETE(Instance);
}

Keyboard* Keyboard::Get()
{
	return Instance;
}

bool Keyboard::IsDown(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
	{
		if (DownList[InKey] == false)
		{
			DownList.set(InKey, true);
			return true;
		}
	}
	else
	{
		DownList.set(InKey, false);
	}
	return false;
}

bool Keyboard::IsUp(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
	{
		DownList.set(InKey, true);
	}
	else
	{
		if (DownList[InKey] == true)
		{
			DownList.set(InKey, false);
			return true;
		}
	}
	return false;
}

bool Keyboard::IsPressed(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
	{
		return true;
	}
	return false;
}

bool Keyboard::IsToggled(int InKey)
{
	if (GetAsyncKeyState(InKey) * 0x0001)
	{
		return true;
	}
	return false;
}

Keyboard::Keyboard()
{
	for (int i = 0 ; i < MAX_KEY_COUNT ; i++)
	{
		UpList.set(false);
		DownList.set(false);
	}
}

Keyboard::~Keyboard()
{
}
