#include "pch.h"
#include "directx/RenderTarget.h"

extern UINT				gBackBufferCount;
extern DXGI_FORMAT		gRTformat;
extern bool				gMsaaEnabled;
extern int				gMsaaQuality;

RenderTarget::RenderTarget()
{

}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::Create(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = gBackBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	CheckHR(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap)));
	
	mRTVIncreasement = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void RenderTarget::Reset(ID3D12Device* device, UINT width, UINT height)
{

}

