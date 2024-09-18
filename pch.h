#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#ifdef _DEBUG
#pragma comment(lib,"Out/debug/Renderer.lib")
#else 
#endif 

#include "Renderer/pch.h"
#include "Renderer/core/window.h"
#include "Renderer/directx/DxRenderer.h"
#include "Renderer/scene/ResourceManager.h"
#include "Renderer/scene/Scene.h"