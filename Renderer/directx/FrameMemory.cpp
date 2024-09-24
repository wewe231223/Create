#include "pch.h"
#include "directx/FrameMemory.h"


//
// 연속적이여야함. 크기가 가변성이여아함 = vector
// 한번 할당받고 복사하는게 시간이 너무 오래걸림 ==> 왠만해서는 하면안됨 
//  
//	Model1								Model2 
//  1 / 2 / 3 / 4 / 5 / 6 / 7 / 8 ....	1 / 2 / 3 / 4 / 5 / 6 / 7 / 8 /  ......  
// 
//  각 모델 별로 크기 제한이 필요함. 
//  
// 
//

FrameMemory::FrameMemory(ComPtr<ID3D12Device>& device)
{
	CheckHR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));
	mCommandAllocator->Reset();
}

void FrameMemory::CheckCommandCompleted(ComPtr<ID3D12Fence>& fence)
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

void FrameMemory::Copy(void* source, EFrameMemoryDest dest, size_t size)
{
	switch (dest)
	{
	case FMD_Object:
		
		break;
	case FMD_Camera:
		break;
	default:
		break;
	}
}

void FrameMemory::SetVariables(ComPtr<ID3D12GraphicsCommandList>& commandList)
{

}

void FrameMemory::Reset()
{
	mCommandAllocator->Reset();
}

void FrameMemory::SetFenceValue(UINT64 value)
{
	mFenceValue = value;
}
