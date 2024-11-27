#include "pch.h"
#include "scene/BoundingBoxRenderer.h"
#include "resource/Shader.h"


BoundingBoxRenderer::BoundingBoxRenderer(ComPtr<ID3D12Device> device)
{
	mShader = std::make_shared<BoundingBoxShader>(device);
}

BoundingBoxRenderer::~BoundingBoxRenderer()
{
}

void BoundingBoxRenderer::SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mShader->SetShader(commandList);
}

void BoundingBoxRenderer::Render(ComPtr<ID3D12GraphicsCommandList>& commandList, UINT instanceCount)
{
	mShader->SetShader(commandList);

	D3D12_VERTEX_BUFFER_VIEW vbv{};

	commandList->IASetVertexBuffers(0, 1, &vbv);
	
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	commandList->DrawInstanced(1, instanceCount, 0, 0);
}
