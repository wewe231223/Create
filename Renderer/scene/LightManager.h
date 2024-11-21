#pragma once 


class LightManager {
	struct Buffer {
		ComPtr<ID3D12Resource> mBuffer{ nullptr };
		LightInfo* mBufferPtr{ nullptr };
	};
public:
	LightManager(ComPtr<ID3D12Device> device);
	~LightManager();
public:
	void SetLight(ComPtr<ID3D12GraphicsCommandList>& commandList);
	Light GetLight();
private:
	UINT mCurrentBuffer{ 0 };
	std::array<Buffer, static_cast<size_t>(EGlobalConstants::GC_FrameCount)> mLightBuffers{};
	
	std::array<LightInfo, static_cast<size_t>(EGlobalConstants::GC_MaxLight)> mLights{};
	Light mNextLight{ mLights.begin() };
};

