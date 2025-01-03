#pragma once

class DxRenderer {
public:
	DxRenderer(std::shared_ptr<class Window> window);
	~DxRenderer();

	// 오로지 ui 를 위해 존재하는 함수 
	ComPtr<ID3D12Device> GetDevice()					const;
	ComPtr<ID3D12GraphicsCommandList> GetCommandList()	const;

	
	void LoadScene(std::shared_ptr<class Scene> scene);
	void UnloadScene();

	void StartRender();
	void Render();
	void EndRender();
private:
	void Initialize();

	void InitDebugCtrl();
	void InitFactory();
	void InitDevice();
	void InitCommandQueue();
	void InitFence();
	void InitSwapChain();
	void InitRenderTargets();
	void InitFrameMemories();
	void InitCommandList();
	void InitShadowCaster();

#ifdef UI_RENDER
	void InitImGui();
	void StartRenderImGui();
	void RenderImGui();
	void TerminateImGui();
#endif 
	void ExecuteCommandList();
	void FlushCommandQueue();
private:
#ifdef _DEBUG
	ComPtr<ID3D12Debug6> mDebugController{ nullptr };
	ComPtr<IDXGIDebug> mDxgiDebug{ nullptr };
#endif
	ComPtr<IDXGIFactory6> mDxgiFactory{ nullptr };
	ComPtr<ID3D12Device> mDevice{ nullptr };

	ComPtr<ID3D12CommandQueue> mCommandQueue{ nullptr };
	ComPtr<ID3D12Fence>	mFence{ nullptr };
	UINT64 mFenceValue{ 0 };

	ComPtr<ID3D12GraphicsCommandList>	mCommandList{ nullptr };
	UINT mCurrentFrameMemoryIndex{ 0 };
	std::array<std::shared_ptr<class FrameMemory>,static_cast<size_t>(EGlobalConstants::GC_FrameCount)> mFrameMemories{};

#ifdef UI_RENDER
	ComPtr<ID3D12DescriptorHeap> mImGuiSrvHeap{ nullptr };
#endif

	std::unique_ptr<class SwapChain> mSwapChain{ nullptr };
	std::unique_ptr<class RenderTargetGroup> mSwapChainRTGroup{ nullptr };

	std::shared_ptr<class Window> mWindow{ nullptr };

	std::shared_ptr<class Scene> mScene{ nullptr };

	std::unique_ptr<class DirectionalShadowCaster> mDirectionalShadowCaster{ nullptr };
};