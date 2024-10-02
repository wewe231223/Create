#include "pch.h"
#include "resource/Mesh.h"
#include "buffer/DefaultBuffer.h"
	
Mesh::Mesh(ComPtr<ID3D12Device>& device, D3D12_PRIMITIVE_TOPOLOGY topology,UINT indexBegin, UINT indexCount)
	:  mTopology(topology), mIndexBegin(indexBegin), mIndexCount(indexCount)
{
	D3D12_RESOURCE_DESC desc{};

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeof(MaterialIndex) * static_cast<UINT64>(GC_MaxRefPerModel);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);

	for (auto& ContextBuffer : mMeshContexts) {
		CheckHR(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(ContextBuffer.mBuffer.GetAddressOf())
		)
		);

		ContextBuffer.mBuffer->Map(0, nullptr, reinterpret_cast<void**>(std::addressof(ContextBuffer.mBufferPtr)));
	}
}

Mesh::~Mesh()
{
}

void Mesh::WriteContext(MaterialIndex mat)
{
	memcpy(mMeshContexts[mMemoryIndex].mBufferPtr + mInstanceCount, &mat, sizeof(MaterialIndex));
	mInstanceCount++;
}

void Mesh::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	commandList->SetGraphicsRootShaderResourceView(GRP_MeshConstants, mMeshContexts[mMemoryIndex].mBuffer->GetGPUVirtualAddress());
	commandList->IASetPrimitiveTopology(mTopology);
	commandList->DrawIndexedInstanced(mIndexCount, mInstanceCount, mIndexBegin, 0, 0);

	mMemoryIndex = (mMemoryIndex + 1) % static_cast<UINT>(GC_FrameCount);
	mInstanceCount = 0;
}

