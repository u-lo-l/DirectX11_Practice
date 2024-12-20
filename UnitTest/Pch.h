#pragma once

#include "Framework.h"
#pragma comment(lib, "Framework.lib")

#define PUSH_MAIN(x) Push(new x())

//이건 뭐야 ㅅㅂ
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")