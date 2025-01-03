#pragma once
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "Framework.h"
#pragma comment(lib, "Framework.lib")

#define PUSH_MAIN(x) Push(new x())
