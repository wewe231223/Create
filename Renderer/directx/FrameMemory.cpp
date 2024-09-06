#include "pch.h"
#include "directx/FrameMemory.h"

FrameMemory::FrameMemory(ComPtr<ID3D12Device> device)
{
	CheckHR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));
	mCommandAllocator->Reset();
}

ComPtr<ID3D12CommandAllocator> FrameMemory::GetAllocator() const
{
	return mCommandAllocator;
}

void FrameMemory::Reset()
{
	mCommandAllocator->Reset();
}
