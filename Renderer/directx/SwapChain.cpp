#include "pch.h"
#include "SwapChain.h"

UINT				gBackBufferCount = 3;
DXGI_FORMAT			gRTformat = DXGI_FORMAT_R8G8B8A8_UNORM;
bool				gMsaaEnabled;
int					gMsaaQuality;