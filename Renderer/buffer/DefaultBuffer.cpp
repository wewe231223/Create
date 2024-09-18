#include "pch.h"
#include "buffer/DefaultBuffer.h"


DefaultBuffer::DefaultBuffer(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, size_t size) {
	CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
	CD3DX12_RESOURCE_DESC bufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(size)) };
	CheckHR(
		device->CreateCommittedResource(
			std::addressof(heapProp),
			D3D12_HEAP_FLAG_NONE,
			std::addressof(bufferDesc),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mUploadBuffer.GetAddressOf())
		)
	);

	mUploadBuffer->SetName(L"DefaultBuffer - Upload");

	heapProp = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT };

	CheckHR(
		device->CreateCommittedResource(
			std::addressof(heapProp),
			D3D12_HEAP_FLAG_NONE,
			std::addressof(bufferDesc),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(mDefaultBuffer.GetAddressOf())
		)
	);

	mDefaultBuffer->SetName(L"DefaultBuffer - Default");
	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData = data;
	subResourceData.RowPitch = size;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	CD3DX12_RESOURCE_BARRIER barrier{ CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST) };

	commandList->ResourceBarrier(1, std::addressof(barrier));

	UpdateSubresources(commandList.Get(), mDefaultBuffer.Get(), mUploadBuffer.Get(), 0, 0, 1, std::addressof(subResourceData));


	barrier = { CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_GENERIC_READ) };

	commandList->ResourceBarrier(1, std::addressof(barrier));


}

DefaultBuffer::~DefaultBuffer() {

}

DefaultBuffer::DefaultBuffer(DefaultBuffer&& other) noexcept {
	mDefaultBuffer = std::move(other.mDefaultBuffer);
	mUploadBuffer = std::move(other.mUploadBuffer);

	other.mDefaultBuffer.Reset();
	other.mUploadBuffer.Reset();
}

DefaultBuffer& DefaultBuffer::operator=(DefaultBuffer&& other) noexcept {
	if (this != std::addressof(other)) {
		mDefaultBuffer = std::move(other.mDefaultBuffer);
		mUploadBuffer = std::move(other.mUploadBuffer);

		other.mDefaultBuffer.Reset();
		other.mUploadBuffer.Reset();
	}
	return *this;
}

ComPtr<ID3D12Resource> DefaultBuffer::GetBuffer() const
{
	return mDefaultBuffer;
}

