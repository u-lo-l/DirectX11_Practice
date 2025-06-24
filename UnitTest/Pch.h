#pragma once

#include "Framework.h"
#pragma comment(lib, "Framework.lib")

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#define PUSH_MAIN(x) Push(new x())
