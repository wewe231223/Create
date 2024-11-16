#include "pch.h"
#include "resource/BillBoardManager.h"

BillBoardManager::BillBoardManager(ComPtr<ID3D12Device> device, ComPtr<ID3D12CommandList> loadCommandList)
{
}

BillBoardManager::~BillBoardManager()
{
}

void BillBoardManager::MakeBillBoard(const BillBoardVertex& vertex)
{
	mVertices.emplace_back(vertex);
	mDecayed = true;
}

void BillBoardManager::MakeBillBoard(const std::vector<BillBoardVertex>& vertices)
{
	std::copy(vertices.begin(), vertices.end(), std::back_inserter(mVertices));
	mDecayed = true;
}

void BillBoardManager::MakeBillBoard(UINT halfWidth, UINT height, TextureIndex image)
{
	BillBoardVertex vertex{};
	vertex.halfWidth = halfWidth;
	vertex.height = height;
	vertex.texture = image;
	vertex.spritable = false;

	mVertices.emplace_back(vertex);

	mDecayed = true;
}

void BillBoardManager::Render(ComPtr<ID3D12GraphicsCommandList>& commandList, D3D12_GPU_VIRTUAL_ADDRESS texHeap)
{
	mShader->SetShader(commandList);

	commandList->SetGraphicsRootShaderResourceView()
}
