#include "pch.h"
#include "scene/LightManager.h"

LightManager::LightManager(ComPtr<ID3D12Device> device)
{
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeof(LightInfo) * static_cast<size_t>(EGlobalConstants::GC_MaxLight);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_HEAP_PROPERTIES heapProp{ CD3DX12_HEAP_PROPERTIES{D3D12_HEAP_TYPE_UPLOAD} };

	for (auto& buffer : mLightBuffers) {
		CheckHR(device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(buffer.mBuffer.GetAddressOf())));
		buffer.mBuffer->Map(0, nullptr, reinterpret_cast<void**>(&buffer.mBufferPtr));
	}
}

LightManager::~LightManager()
{
}


void LightManager::SetLight(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	::memcpy(mLightBuffers[mCurrentBuffer].mBufferPtr, mLights.data(), sizeof(LightInfo) * mLights.size());

	commandList->SetGraphicsRootShaderResourceView(static_cast<UINT>(EGraphicRootParamIndex::GRP_LightInfo), mLightBuffers[mCurrentBuffer].mBuffer->GetGPUVirtualAddress());

	mCurrentBuffer = (mCurrentBuffer + 1) % static_cast<size_t>(EGlobalConstants::GC_FrameCount);
}

Light LightManager::GetLight()
{
	if (mNextLight == mLights.end()) {
		mNextLight = mLights.begin();
	}
	return mNextLight++;
}
