#pragma once

class DxRenderer {
	static constexpr UINT gFrameCount = 3;
public:
	DxRenderer(std::shared_ptr<class Window> window);
	~DxRenderer();

	// 오로지 ui 를 위해 존재하는 함수 
	ComPtr<ID3D12Device> GetDevice()					const;
	ComPtr<ID3D12GraphicsCommandList> GetCommandList()	const;


	void StartRender();
	void EndRender();
private:
	void Initialize();

	void InitDebugCtrl();
	void InitFactory();
	void InitDevice();
	void InitSwapChain();
	void InitCommandQueue();
	void InitFence();
	void InitFrameMemories();
	void InitCommandList();
private:
#ifdef _DEBUG
	ComPtr<ID3D12Debug6> mDebugController{ nullptr };
	ComPtr<IDXGIDebug> mDxgiDebug{ nullptr };
#endif
	ComPtr<IDXGIFactory6> mDxgiFactory{ nullptr };
	ComPtr<ID3D12Device> mDevice{ nullptr };

	ComPtr<ID3D12CommandQueue> mCommandQueue{ nullptr };
	ComPtr<ID3D12Fence>	mFence{ nullptr };

	ComPtr<ID3D12GraphicsCommandList> mCommandList{ nullptr };
	UINT mCurrentFrameMemoryIndex{ 0 };
	std::array<std::unique_ptr<class FrameMemory>,gFrameCount> mFrameMemories{};

	UINT64 mFenceValue{ 0 };

	std::shared_ptr<class Window> mWindow{ nullptr };
};