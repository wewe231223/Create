#include "pch.h"
#include "resource/Mesh.h"

	
Mesh::Mesh(D3D12_PRIMITIVE_TOPOLOGY topology,UINT indexBegin, UINT indexCount, UINT materialIndex)
	:  mTopology(topology), mIndexBegin(indexBegin), mIndexCount(indexCount), mMaterialIndex(materialIndex)
{
}

Mesh::~Mesh()
{
}

void Mesh::Render(ComPtr<ID3D12GraphicsCommandList>& commandList, UINT instance) const
{
	commandList->IASetPrimitiveTopology(mTopology);
	commandList->DrawIndexedInstanced(mIndexCount, instance, mIndexBegin, 0, 0);
}

