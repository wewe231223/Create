#include "pch.h"
#include "core/window.h"
#include "directx/DxRenderer.h"
#include "directx/FrameMemory.h"
#include "directx/SwapChain.h"
#include "directx/RenderTarget.h"
#include "ui/Console.h"
#include "resource/Shader.h"
#include "scene/Scene.h"
#include "buffer/DefaultBuffer.h"
// 주석을 셋중 하나는 제거하고 사용할 것 
// constexpr const char* KoreanFontPath = "C://Windows//Fonts//malgun.ttf";
// constexpr const char* KoreanFontPath = "Resources/font/MaruBuri-Regular.ttf";
 constexpr const char* KoreanFontPath = "Resources/font/NotoSansKR-Regular-Hestia.otf";

DxRenderer::DxRenderer(std::shared_ptr<Window> window) 
	: mWindow(window)
{
	DxRenderer::Initialize();

	Time.AddEvent(1s, []() {
		ImGuiIO& io = ImGui::GetIO();
		Console.InfoLog("FrameRate : {:.2f}", io.Framerate);
		return true;
		});
	Console.InfoLog("이제 로그 메세지는 한글 문자도 지원합니다.");

	
}

DxRenderer::~DxRenderer()
{
	FlushCommandQueue();
	DxRenderer::TerminateImGui();

}

ComPtr<ID3D12Device> DxRenderer::GetDevice() const
{
	return mDevice;
}

ComPtr<ID3D12GraphicsCommandList> DxRenderer::GetCommandList() const
{
	return mCommandList;
}

void DxRenderer::LoadScene(std::shared_ptr<class Scene> scene)
{
	if (mScene) {
		Console.WarningLog("이미 로드되어 있는 Scene : {} 이 있습니다.",scene->GetName());
	}
	else {
		Console.InfoLog("Scene : {} 을 로드합니다.", scene->GetName());

	
		mLoadCommandAllocator->Reset();
		mCommandList->Reset(mLoadCommandAllocator.Get(), nullptr);

		scene->Load(mDevice, mCommandList);
		mScene = scene;
		
		DxRenderer::ExecuteCommandList();
		DxRenderer::FlushCommandQueue();
	}
}

void DxRenderer::UnloadScene()
{
	Console.InfoLog("Scene : {} 을 언로드합니다.", mScene->GetName());
	mScene.reset();

	
}

void DxRenderer::StartRender()
{
	mCurrentFrameMemoryIndex = (mCurrentFrameMemoryIndex + 1) % static_cast<UINT>(EGlobalConstants::GC_FrameCount);
	auto& frameMemory = mFrameMemories[mCurrentFrameMemoryIndex];
	
	frameMemory->CheckCommandCompleted(mFence);
	
	frameMemory->Reset();
	mCommandList->Reset(frameMemory->GetAllocator().Get(),nullptr);
	auto backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	mSwapChainRTGroup->SetRTState(mCommandList, backBufferIndex, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mSwapChainRTGroup->Clear(mCommandList, backBufferIndex);
	mSwapChainRTGroup->SetRenderTarget(mCommandList, backBufferIndex);

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
	DxRenderer::StartRenderImGui();

}


void DxRenderer::Render()
{
#ifdef UI_RENDER
	Console.Render();

#endif 
	if (mScene)
		mScene->Render(mCommandList);
}

void DxRenderer::EndRender()
{
	DxRenderer::RenderImGui();
	mSwapChainRTGroup->SetRTState(mCommandList, mSwapChain->GetCurrentBackBufferIndex(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
	DxRenderer::ExecuteCommandList();

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(nullptr, reinterpret_cast<void*>(mCommandList.Get()));
	}

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
#ifdef UI_RENDER
	DxRenderer::InitImGui();
#endif 
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
		std::vector<ComPtr<ID3D12Resource>> rts{ static_cast<size_t>(EGlobalConstants::GC_BackBufferCount) };
		for (UINT i = 0; i < static_cast<UINT>(EGlobalConstants::GC_BackBufferCount); ++i) {
			CheckHR(mSwapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(rts[i].GetAddressOf())));
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
	CheckHR(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mLoadCommandAllocator.GetAddressOf())));
	CheckHR(mDevice->CreateCommandList(
		0, 
		D3D12_COMMAND_LIST_TYPE_DIRECT, 
		mFrameMemories[mCurrentFrameMemoryIndex]->GetAllocator().Get(), 
		nullptr, 
		IID_PPV_ARGS(&mCommandList))
	);
	mCommandList->Close();
}

void DxRenderer::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

#ifdef UI_DARK_THEME
	ImGui::StyleColorsDark();
#else 
	ImGui::StyleColorsLight();
#endif 
	ImGuiStyle& style = ImGui::GetStyle();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NumDescriptors = 1;
	desc.NodeMask = 0;
	CheckHR(mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mImGuiSrvHeap.GetAddressOf())));

	auto imwin32 = ImGui_ImplWin32_Init(mWindow->GetWindowHandle());
	auto imdx12 = ImGui_ImplDX12_Init(
		mDevice.Get(), 
		static_cast<int>(EGlobalConstants::GC_FrameCount),
		static_cast<DXGI_FORMAT>(EGlobalConstants::GC_RenderTargetFormat), 
		mImGuiSrvHeap.Get(), 
		mImGuiSrvHeap->GetCPUDescriptorHandleForHeapStart(), 
		mImGuiSrvHeap->GetGPUDescriptorHandleForHeapStart()
	);

	if (!imdx12 || !imwin32) {
		abort();
	}


	ImFontConfig fontConfig{};
	fontConfig.OversampleH = 3;
	fontConfig.OversampleV = 1;
	fontConfig.PixelSnapH = true;
	fontConfig.MergeMode = false;
	io.Fonts->AddFontFromFileTTF(KoreanFontPath, 24.f, &fontConfig, io.Fonts->GetGlyphRangesKorean());
	io.Fonts->Build();
}

void DxRenderer::StartRenderImGui()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void DxRenderer::RenderImGui()
{
	ImGui::Render();
	mCommandList->SetDescriptorHeaps(1, mImGuiSrvHeap.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());

}

void DxRenderer::TerminateImGui()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (mImGuiSrvHeap) mImGuiSrvHeap.Reset();	
}

void DxRenderer::ExecuteCommandList()
{
	CheckHR(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
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
