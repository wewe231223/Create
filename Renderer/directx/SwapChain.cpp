#include "pch.h"
#include "SwapChain.h"
#include "core/window.h"

#define SWAPCHAIN_ALLOW_TEARING

UINT				gBackBufferCount = 3;
DXGI_FORMAT			gRTformat = DXGI_FORMAT_R8G8B8A8_UNORM;

SwapChain::SwapChain(std::shared_ptr<Window> window, ComPtr<IDXGIFactory6> factory, ComPtr<ID3D12CommandQueue> commandQueue)
{
	SwapChain::Create(window, factory, commandQueue);
}

SwapChain::~SwapChain()
{
}

ComPtr<IDXGISwapChain4> SwapChain::GetSwapChain()
{
	return mSwapChain;
}

void SwapChain::Create(std::shared_ptr<class Window> window, ComPtr<IDXGIFactory6> factory, ComPtr<ID3D12CommandQueue> commandQueue)
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 sd{};
	sd.BufferCount = gBackBufferCount;
	sd.Width = window->GetWindowWidth<UINT>();
	sd.Height = window->GetWindowHeight<UINT>();
	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼 형식
	sd.Stereo = FALSE; // 스테레오 모드 비활성화
	sd.SampleDesc.Count = 1; // 멀티샘플링 비활성화
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 버퍼 사용 방법
	sd.Scaling = DXGI_SCALING_STRETCH; // 스케일링 모드
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 효과
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // 알파 모드
#ifdef SWAPCHAIN_ALLOW_TEARING
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING; // 추가 옵션 없음
#else 
	sd.Flags = 0;
#endif 
	ComPtr<IDXGISwapChain1> temp{};

	CheckHR(
		factory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			window->GetWindowHandle(),
			&sd,
			nullptr,
			nullptr,
			temp.GetAddressOf()
		)
	);


	CheckHR(temp.As(&mSwapChain));
}

UINT SwapChain::GetCurrentBackBufferIndex() const
{
	return mCurrentBackBufferIndex;
}

void SwapChain::Present()
{
#ifdef SWAPCHAIN_ALLOW_TEARING 
	CheckHR(mSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
#else 
	CheckHR(mSwapChain->Present(0, 0));
#endif 
	mCurrentBackBufferIndex = (mCurrentBackBufferIndex + 1) % gBackBufferCount;
}


