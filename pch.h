#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ 런타임 헤더 파일입니다. 
#include <random>
#include <numeric>
#include <bitset>
#include <concepts>
#include <ranges>
#include <typeindex>

#define STAND_ALONE

#ifdef _DEBUG
#pragma comment(lib,"Out/debug/Renderer.lib")
#else 
#pragma comment(lib,"Out/release/Renderer.lib")
#endif 

#ifndef STAND_ALONE
// 11-02 김성준 추가 - 클라이언트의 네트워크 작업을 위한 헤더 및 라이브러리 추가
#include "ClientNetwork/pch.h"

#ifdef _DEBUG
#pragma comment(lib, "Out/debug/ClientNetwork.lib")
#else
#pragma comment(lib, "Out/release/ClientNetwork.lib")
#endif
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
#include "Renderer/ui/UIRenderer.h"
#include "Renderer/ui/Console.h"

#include "Game/defines.h"
#include "Game/utils/Interface.h"
#include "Game/utils/ObjectPool.h"
#include "Game/utils/NonReplacementSampler.h"
#include "Game/utils/Input.h"
#include "Game/utils/Transform.h"
#include "Game/scene/Camera.h"
#include "Game/gameobject/UIObject.h"
#include "Game/gameobject/GameObject.h"