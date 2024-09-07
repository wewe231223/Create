#include "pch.h"
#include "directx/FrameMemory.h"

FrameMemory::FrameMemory(ComPtr<ID3D12Device> device)
{
	CheckHR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));
	mCommandAllocator->Reset();
}

void FrameMemory::CheckCommandCompleted(ComPtr<ID3D12Fence> fence)
{
	if (mFenceValue != 0 && mFenceValue > fence->GetCompletedValue()){
		HANDLE event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		CheckHR(fence->SetEventOnCompletion(mFenceValue, event));
		::WaitForSingleObject(event, INFINITE);
		::CloseHandle(event);
	}
}

ComPtr<ID3D12CommandAllocator> FrameMemory::GetAllocator() const
{
	return mCommandAllocator;
}

void FrameMemory::Reset()
{
	mCommandAllocator->Reset();
}

void FrameMemory::SetFenceValue(UINT64 value)
{
	mFenceValue = value;
}
