#include "pch.h"
#include "ui/UIRenderer.h"
#include "buffer/DefaultBuffer.h"
#include "resource/Shader.h"


UIRenderer::UIRenderer(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& loadCommandList)
	: mDevice(device), mLoadCommandList(loadCommandList)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = static_cast<UINT>(EGlobalConstants::GC_MaxUiImageNumber);
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	CheckHR(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mTexHeap.GetAddressOf())));



	for (auto& context : mContexts)
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = sizeof(ModelContext2D) * static_cast<UINT64>(GC_MaxRefPerModel);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;


		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);

		CheckHR(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(context.mBuffer.GetAddressOf())));

		context.mBuffer->Map(0, nullptr, reinterpret_cast<void**>(&context.mContext));
	}


	mShader = std::make_unique<UIShader>(device);

	std::vector<DirectX::XMFLOAT2> vertices{
		{0.0f, 0.0f},  // Bottom-left
		{1.0f, 0.0f},  // Bottom-right
		{1.0f, 1.0f},  // Top-right
		{0.0f, 1.0f},  // Top-left
	};

	std::vector<UINT> indices{
		0, 1, 3,
		1, 2, 3
	};


	mVertexBuffer = std::make_unique<DefaultBuffer>(device, loadCommandList, vertices.data() , sizeof(DirectX::XMFLOAT2) * vertices.size());

	mVertexBufferView.BufferLocation = mVertexBuffer->GetBuffer()->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(DirectX::XMFLOAT2));
	mVertexBufferView.SizeInBytes = static_cast<UINT>(sizeof(DirectX::XMFLOAT2) * vertices.size());

	mIndexBuffer = std::make_unique<DefaultBuffer>(device, loadCommandList, indices.data(), sizeof(UINT) * indices.size());

	mIndexBufferView.BufferLocation = mIndexBuffer->GetBuffer()->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	mIndexBufferView.SizeInBytes = static_cast<UINT>(sizeof(UINT) * indices.size());
}

UIRenderer::~UIRenderer()
{
}

void UIRenderer::CreateUIImage(const std::string& name, const fs::path& path)
{
	auto newTexture = mTextures.emplace_back(std::make_unique<DefaultBuffer>(mDevice, mLoadCommandList, path))->GetBuffer();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = newTexture->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// TODO : 밉레벨은? 
	srvDesc.Texture2D.MipLevels = 1;

	CD3DX12_CPU_DESCRIPTOR_HANDLE texheapHandle{ mTexHeap->GetCPUDescriptorHandleForHeapStart() };
	texheapHandle.Offset(static_cast<UINT>(mTextures.size() - 1), mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	mDevice->CreateShaderResourceView(newTexture.Get(), &srvDesc, texheapHandle);

	mTextureMap[name] = static_cast<TextureIndex>(mTextures.size() - 1);
}

TextureIndex UIRenderer::GetUIImage(const std::string& name)
{
	if (mTextureMap.find(name) == mTextureMap.end()) {
		return NULL_INDEX;
	}

	return mTextureMap[name];
}

void UIRenderer::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mShader->SetShader(commandList);

	commandList->SetDescriptorHeaps(1, mTexHeap.GetAddressOf());

	commandList->SetGraphicsRootShaderResourceView(URP_ObjectConstants, mContexts[mMemoryIndex].mBuffer->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(URP_Texture, mTexHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
	commandList->IASetIndexBuffer(&mIndexBufferView);

	commandList->DrawIndexedInstanced(6, mInstanceCount, 0, 0, 0);

	mMemoryIndex = (mMemoryIndex + 1) % static_cast<size_t>(GC_FrameCount);
	mInstanceCount = 0;
}

void UIRenderer::WriteContext(ModelContext2D* data)
{
	std::memcpy(mContexts[mMemoryIndex].mContext, data, sizeof(ModelContext2D));
	mInstanceCount += 1;
}
