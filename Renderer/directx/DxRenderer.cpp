#include "pch.h"
#include "core/window.h"
#include "directx/DxRenderer.h"
#include "directx/FrameMemory.h"
#include "directx/SwapChain.h"
#include "directx/RenderTarget.h"

extern UINT gBackBufferCount;

DxRenderer::DxRenderer(std::shared_ptr<Window> window) 
	: mWindow(window)
{
	DxRenderer::Initialize();
}

DxRenderer::~DxRenderer()
{
	FlushCommandQueue();

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
	mCurrentFrameMemoryIndex = (mCurrentFrameMemoryIndex + 1) % gFrameCount;
	auto& frameMemory = mFrameMemories[mCurrentFrameMemoryIndex];
	
	frameMemory->CheckCommandCompleted(mFence.Get());
	
	frameMemory->Reset();
	mCommandList->Reset(frameMemory->GetAllocator().Get(),nullptr);
	auto backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	mSwapChainRTGroup->SetRTState(mCommandList.Get(), backBufferIndex, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mSwapChainRTGroup->Clear(mCommandList.Get(), backBufferIndex);

	D3D12_VIEWPORT vp{};
	vp.Height = mWindow->GetWindowHeight<float>();
	vp.Width = mWindow->GetWindowWidth<float>();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = mWindow->GetWindowWidth<long>();
	scissorRect.bottom = mWindow->GetWindowHeight<long>();

	mCommandList->RSSetViewports(1, &vp);
	mCommandList->RSSetScissorRects(1, &scissorRect);

}

void DxRenderer::Render()
{
	mSwapChainRTGroup->Clear(mCommandList, mSwapChain->GetCurrentBackBufferIndex());
}

void DxRenderer::EndRender()
{
	mSwapChainRTGroup->SetRTState(mCommandList.Get(), mSwapChain->GetCurrentBackBufferIndex(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CheckHR(mCommandList->Close());

	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	mSwapChain->Present();

	mFrameMemories[mCurrentFrameMemoryIndex]->SetFenceValue(++mFenceValue);
	mCommandQueue->Signal(mFence.Get(), mFenceValue);
}

void DxRenderer::Initialize()
{
	DxRenderer::InitDebugCtrl();
	DxRenderer::InitFactory();
	DxRenderer::InitDevice();
	DxRenderer::InitCommandQueue();
	DxRenderer::InitFence();
	DxRenderer::InitSwapChain();
	DxRenderer::InitRenderTargets();
	DxRenderer::InitFrameMemories();
	DxRenderer::InitCommandList();

	FlushCommandQueue();
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
	// SwapChain 에서 사용할 렌더 타겟 그룹 생성
	{
		std::vector<ComPtr<ID3D12Resource>> rts{ gBackBufferCount };
		for (UINT i = 0; i < gBackBufferCount; ++i) {
			CheckHR(mSwapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&rts[i])));
		}

		ComPtr<ID3D12Resource> ds{ nullptr };
		auto dsDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT, 
			mWindow->GetWindowWidth<UINT>(), 
			mWindow->GetWindowHeight<UINT>(), 
			1, 
			1, 
			1, 
			0, 
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

		CD3DX12_CLEAR_VALUE clearValue{ DXGI_FORMAT_D32_FLOAT, 1.0f, 0 };
		auto heapproperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		CheckHR(mDevice->CreateCommittedResource(
			&heapproperties,
			D3D12_HEAP_FLAG_NONE,
			&dsDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(ds.GetAddressOf()))
		);

		mSwapChainRTGroup = std::make_unique<RenderTargetGroup>();
		mSwapChainRTGroup->Create(mDevice, rts, ds, DirectX::SimpleMath::Vector4{ 0.0f, 0.0f, 0.0f, 1.0f });
	}

}

void DxRenderer::InitFrameMemories()
{
	for (auto& frameMemory : mFrameMemories)
	{
		frameMemory = std::make_shared<FrameMemory>(mDevice);
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

void DxRenderer::FlushCommandQueue()
{
	mFenceValue++;
	CheckHR(mCommandQueue->Signal(mFence.Get(), mFenceValue));
	if (mFence->GetCompletedValue() < mFenceValue) {
		HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		mFence->SetEventOnCompletion(mFenceValue, eventHandle);
		::WaitForSingleObject(eventHandle, INFINITE);
		::CloseHandle(eventHandle);
	}
}
