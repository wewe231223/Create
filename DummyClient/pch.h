#pragma once

#include "ClientNetwork/pch.h"

#ifdef _DEBUG || DEBUG
#pragma comment(lib, "out/debug/ClientNetwork.lib")
#else
#pragma comment(lib, "out/release/ClientNetwork.lib")
#endif