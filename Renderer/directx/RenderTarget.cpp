#include "pch.h"
#include "directx/RenderTarget.h"


RenderTargetGroup::RenderTargetGroup()
{
}

RenderTargetGroup::~RenderTargetGroup()
{
}

void RenderTargetGroup::Create(ComPtr<ID3D12Device> device, std::vector<ComPtr<ID3D12Resource>>& rts, ComPtr<ID3D12Resource>& ds, const DirectX::SimpleMath::Vector4& clearColor)
{
	mRenderTargets = std::move(rts);
	mDepthStencil = std::move(ds);
	mRTVDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mRTVClearColor = clearColor;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = static_cast<UINT>(mRenderTargets.size());
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	CheckHR(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	CheckHR(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap)));

	D3D12_CPU_DESCRIPTOR_HANDLE handle{ mRtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (auto& rt : mRenderTargets) {
		device->CreateRenderTargetView(rt.Get(), nullptr, handle);
		handle.ptr += mRTVDescriptorSize;
	}


	device->CreateDepthStencilView(mDepthStencil.Get(), nullptr, mDsvHeap->GetCPUDescriptorHandleForHeapStart());

}

void RenderTargetGroup::Reset()
{
	mRtvHeap->Release();

	for (auto& rt : mRenderTargets) {
		rt->Release();
	}

	mDepthStencil->Release();
}

void RenderTargetGroup::SetRTState(ComPtr<ID3D12GraphicsCommandList> commandlist, UINT index, D3D12_RESOURCE_STATES prev, D3D12_RESOURCE_STATES next)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[index].Get(), prev, next);
	commandlist->ResourceBarrier(1, &barrier);
}

void RenderTargetGroup::SetRTState(ComPtr<ID3D12GraphicsCommandList> commandlist, D3D12_RESOURCE_STATES prev, D3D12_RESOURCE_STATES next)
{
	D3D12_RESOURCE_BARRIER barriers[8]{};
	for (auto i = 0; i < mRenderTargets.size(); ++i) {
		barriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[i].Get(), prev, next);
	}
	commandlist->ResourceBarrier(static_cast<UINT>(mRenderTargets.size()), barriers);
}

void RenderTargetGroup::SetRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList, UINT index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ mRtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += mRTVDescriptorSize * index;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ mDsvHeap->GetCPUDescriptorHandleForHeapStart() };

	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void RenderTargetGroup::SetRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ mRtvHeap->GetCPUDescriptorHandleForHeapStart() };
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ mDsvHeap->GetCPUDescriptorHandleForHeapStart() };

	commandList->OMSetRenderTargets(static_cast<UINT>(mRenderTargets.size()), &rtvHandle, TRUE, &dsvHandle);
}

void RenderTargetGroup::Clear(ComPtr<ID3D12GraphicsCommandList> commandList, UINT index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle{ mRtvHeap->GetCPUDescriptorHandleForHeapStart() };
	handle.Offset(index, mRTVDescriptorSize);

	float clearColor[] = { mRTVClearColor.x, mRTVClearColor.y, mRTVClearColor.z, mRTVClearColor.w };
	commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(mDsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderTargetGroup::Clear(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle{ mRtvHeap->GetCPUDescriptorHandleForHeapStart() };
	float clearColor[] = { mRTVClearColor.x, mRTVClearColor.y, mRTVClearColor.z, mRTVClearColor.w };

	for (auto i = 0; i < mRenderTargets.size(); ++i) {
		commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
		handle.Offset(mRTVDescriptorSize);
	}
	commandList->ClearDepthStencilView(mDsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}


