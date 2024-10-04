#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <random>
#include <numeric>
#include <bitset>
#include <concepts>

#ifdef _DEBUG
#pragma comment(lib,"Out/debug/Renderer.lib")
#else 
#endif 

#include "Renderer/external/Include/DirectXTK12/Keyboard.h"
#include "Renderer/external/Include/DirectXTK12/Mouse.h"

#include "Renderer/pch.h"
#include "Renderer/core/window.h"
#include "Renderer/directx/DxRenderer.h"
#include "Renderer/scene/Scene.h"

#include "Renderer/scene/ResourceManager.h"
#include "Renderer/resource/Shader.h"
#include "Renderer/resource/Model.h"
#include "Renderer/resource/Mesh.h"
#include "Renderer/buffer/DefaultBuffer.h"
#include "Renderer/ui/Console.h"


#include "Game/GameObject.h"
#include "Game/components/Transform.h"
#include "Game/Input.h"
