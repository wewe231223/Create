#pragma once

// WinSock
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Windows
#include <Windows.h>
#include <stdlib.h>

// C++ 표준 헤더
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <string_view>

#include <numeric>
#include <algorithm>

#include <ranges>
#include <concepts>

#include <queue>
#include <array>
#include <vector>

/* concurrency support */
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>