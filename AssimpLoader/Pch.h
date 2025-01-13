#pragma once
// #pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "Framework.h"
#pragma comment(lib, "Framework.lib")

#include "Assimp/Importer.hpp"
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
#include "Assimp/postprocess.h"
#include "Assimp/scene.h"

#include "JsonCpp/json.h"
#pragma comment(lib, "JsonCpp/jsoncpp_static.lib")


#define PUSH_MAIN(x) Push(new x())
