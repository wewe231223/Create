// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// DiectX12 Header
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <comdef.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>

#include "external/Include/DirectXTK12/d3dx12.h"
#include "external/Include/DirectXTex/DirectXTex.h"
#include "external/Include/DirectXTex/DirectXTex.inl"

// DirectX12 Library 
#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#ifdef _DEBUG 
#pragma comment(lib,"Renderer/external/lib/debug/DirectXTK12.lib")
#pragma comment(lib,"Renderer/external/lib/debug/DirectXTex.lib")
#else
#pragma comment(lib,"Renderer/external/lib/release/DirectXTK12.lib")
#pragma comment(lib,"Renderer/external/lib/release/DirectXTex.lib")
#endif
#include <optional>
#include <string>
#include <memory>
#include <format>
#include <source_location>

using namespace Microsoft::WRL;
#include "core/Utility.h"