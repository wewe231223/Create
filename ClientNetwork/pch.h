#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <Windows.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>