#include "pch.h"
#include "resource/BillBoard.h"
#include "resource/Shader.h"

BillBoard::BillBoard(ComPtr<ID3D12Device> device, ComPtr<ID3D12CommandList> loadCommandList)
{
	mShader = std::make_unique<BillBoardShader>(device);

	// 빌보드 정점 업로드 버퍼 생성... 굳이 업로드 버퍼어야?  
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = sizeof(BillBoardVertex) * static_cast<UINT64>(BillboardCount);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;


	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);

	for (auto& buffer : mVertexBuffers)
	{
		CheckHR(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(buffer.buffer.GetAddressOf())));
		buffer.buffer->Map(0, nullptr, reinterpret_cast<void**>(&buffer.bufferptr));
	}

	mVertices.reserve(10'0000);
}

BillBoard::~BillBoard()
{
}

void BillBoard::MakeBillBoard(const BillBoardVertex& vertex)
{
	mVertices.emplace_back(vertex);
	mDecayed = true;

	for (auto& buffer : mVertexBuffers) {
		::memcpy(buffer.bufferptr, mVertices.data(), sizeof(BillBoardVertex) * mVertices.size());

	}

}

void BillBoard::MakeBillBoard(const std::vector<BillBoardVertex>& vertices)
{
	std::copy(vertices.begin(), vertices.end(), std::back_inserter(mVertices));
	mDecayed = true;
	for (auto& buffer : mVertexBuffers) {
		::memcpy(buffer.bufferptr, mVertices.data(), sizeof(BillBoardVertex) * mVertices.size());

	}
}

void BillBoard::MakeBillBoard(const DirectX::XMFLOAT3& position, UINT halfWidth, UINT height, TextureIndex image)
{
	BillBoardVertex vertex{};
	vertex.position = position;
	vertex.halfWidth = halfWidth;
	vertex.height = height;
	vertex.texture = image;
	vertex.spritable = false;

	mVertices.emplace_back(vertex);

	mDecayed = true;
}

void BillBoard::Render(ComPtr<ID3D12GraphicsCommandList>& commandList, D3D12_GPU_DESCRIPTOR_HANDLE texHeap, D3D12_GPU_VIRTUAL_ADDRESS cameraBuffer)
{
	if (mDecayed) {
		mDecayed = false;
	}
	// TODO :: Decay 인 상태를 판별하라. 
//	::memcpy(mVertexBuffers[mCurrentBuffer].bufferptr, mVertices.data(), sizeof(BillBoardVertex) * mVertices.size());
	
	mShader->SetShader(commandList);

	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = mVertexBuffers[mCurrentBuffer].buffer->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(BillBoardVertex);
	vbv.SizeInBytes = static_cast<UINT>(sizeof(BillBoardVertex) * mVertices.size());

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	commandList->SetGraphicsRootConstantBufferView(BRP_CameraConstants, cameraBuffer);

	commandList->SetGraphicsRoot32BitConstant(BRP_Time, Time.GetTimeSinceStarted<UINT>(), 0);

	commandList->SetGraphicsRootDescriptorTable(BRP_Texture, texHeap);

	commandList->DrawInstanced(static_cast<UINT>(mVertices.size()), 1, 0, 0);

	mCurrentBuffer = (mCurrentBuffer + 1) % static_cast<size_t>(GC_FrameCount);

}
