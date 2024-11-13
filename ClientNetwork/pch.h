#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <Windows.h>

#undef min
#undef max

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <source_location>

#include <string>
#include <string_view>

#include <array>
#include <span>

#include "../Renderer/external/Include/DirectXTK12/SimpleMath.h"

#ifdef _DEBUG 
#pragma comment(lib,"../Renderer/external/lib/debug/DirectXTK12.lib")
#else
#pragma comment(lib,"../Renderer/external/lib/release/DirectXTK12.lib")
#endif

#include "core/Protocol.h"