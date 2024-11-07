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

#include "core/Protocol.h"