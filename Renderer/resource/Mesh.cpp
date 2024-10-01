#include "pch.h"
#include "resource/Mesh.h"
#include "buffer/DefaultBuffer.h"
	
Mesh::Mesh(D3D12_PRIMITIVE_TOPOLOGY topology,UINT indexBegin, UINT indexCount)
	:  mTopology(topology), mIndexBegin(indexBegin), mIndexCount(indexCount)
{
}

Mesh::~Mesh()
{
}

void Mesh::Ref(const MaterialIndex& material)
{
	mMaterials.emplace_back(material);
}

void Mesh::UploadMaterialInfo(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (mMaterials.empty()) {
		return;
	}
	auto size = sizeof(MaterialIndex) * mMaterials.size();
	mMaterialBuffer = std::make_unique<DefaultBuffer>(device, commandList, mMaterials.data(), sizeof(MaterialIndex) * mMaterials.size());
}

void Mesh::Render(ComPtr<ID3D12GraphicsCommandList>& commandList, UINT instance) const
{
	commandList->SetGraphicsRootShaderResourceView(GRP_MeshConstants, mMaterialBuffer->GetBuffer()->GetGPUVirtualAddress());
	commandList->IASetPrimitiveTopology(mTopology);
	commandList->DrawIndexedInstanced(mIndexCount, instance, mIndexBegin, 0, 0);
}

