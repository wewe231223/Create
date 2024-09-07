#include "pch.h"
#include "directx/DxRenderer.h"
#include "directx/FrameMemory.h"
#include "directx/SwapChain.h"
#include "directx/RenderTarget.h"

extern UINT gBackBufferCount;

DxRenderer::DxRenderer(std::shared_ptr<class Window> window) 
	: mWindow(window)
{

}

DxRenderer::~DxRenderer()
{
}

ComPtr<ID3D12Device> DxRenderer::GetDevice() const
{
	return mDevice;
}

ComPtr<ID3D12GraphicsCommandList> DxRenderer::GetCommandList() const
{
	return mCommandList;
}

void DxRenderer::StartRender()
{
}

void DxRenderer::EndRender()
{
}

void DxRenderer::Initialize()
{
}

void DxRenderer::InitDebugCtrl()
{
	CheckHR(D3D12GetDebugInterface(IID_PPV_ARGS(&mDebugController)));
	mDebugController->EnableDebugLayer();
	CheckHR(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&mDxgiDebug)));
}

void DxRenderer::InitFactory()
{
	CheckHR(CreateDXGIFactory1(IID_PPV_ARGS(&mDxgiFactory)));
}

void DxRenderer::InitDevice()
{
	HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));

	if (FAILED(hr))
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		CheckHR(mDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		CheckHR(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice)));
	}

}

void DxRenderer::InitCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CheckHR(mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCommandQueue)));
}

void DxRenderer::InitFence()
{
	CheckHR(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
}

void DxRenderer::InitSwapChain()
{
	mSwapChain = std::make_unique<SwapChain>(mWindow, mDxgiFactory, mCommandQueue);
}

void DxRenderer::InitRenderTargets()
{
	{
		std::vector<ComPtr<ID3D12Resource>> rts{ gBackBufferCount };
		for (auto i = 0; i < gBackBufferCount; ++i){
			CheckHR(mSwapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&rts[i])));
		}

	}

}

void DxRenderer::InitFrameMemories()
{
	for (auto& frameMemory : mFrameMemories)
	{
		frameMemory = std::make_unique<FrameMemory>(mDevice);
	}
}

void DxRenderer::InitCommandList()
{
	CheckHR(mDevice->CreateCommandList(
		0, 
		D3D12_COMMAND_LIST_TYPE_DIRECT, 
		mFrameMemories[mCurrentFrameMemoryIndex]->GetAllocator().Get(), 
		nullptr, 
		IID_PPV_ARGS(&mCommandList))
	);
	mCommandList->Close();
}
