#pragma once 
class SwapChain {
public:
	SwapChain(std::shared_ptr<class Window> window,ComPtr<IDXGIFactory6> factory,ComPtr<ID3D12CommandQueue> commandQueue);
	~SwapChain();

	ComPtr<IDXGISwapChain4> GetSwapChain();

	void Create(std::shared_ptr<class Window> window, ComPtr<IDXGIFactory6> factory, ComPtr<ID3D12CommandQueue> commandQueue);
	UINT GetCurrentBackBufferIndex() const;
	void Present();
private:
private:
	ComPtr<IDXGISwapChain4> mSwapChain{ nullptr };
	UINT mCurrentBackBufferIndex{ 0 };
};