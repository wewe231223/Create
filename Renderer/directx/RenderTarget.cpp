#include "pch.h"
#include "directx/RenderTarget.h"


RenderTargetGroup::RenderTargetGroup(ComPtr<ID3D12Device> device, std::vector<ComPtr<ID3D12Resource>>& rts, ComPtr<ID3D12Resource>& ds)
{
	mRenderTargets = std::move(rts);
	mDepthStencil = std::move(ds);



}

RenderTargetGroup::~RenderTargetGroup()
{
}
