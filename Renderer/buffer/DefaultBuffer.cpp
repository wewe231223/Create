#include "pch.h"
#include "buffer/DefaultBuffer.h"
#include "ui/Console.h"

DefaultBuffer::DefaultBuffer(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, size_t size) {

	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT };
	CD3DX12_RESOURCE_DESC bufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(size)) };

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

	heapProp = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_UPLOAD };

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

	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData = data;
	subResourceData.RowPitch = size;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	CD3DX12_RESOURCE_BARRIER barrier{ CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST) };
	
	commandList->ResourceBarrier(1, std::addressof(barrier));

	UpdateSubresources(commandList.Get(), mDefaultBuffer.Get(), mUploadBuffer.Get(), 0, 0, 1, std::addressof(subResourceData));

	barrier = { CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_GENERIC_READ) };

	commandList->ResourceBarrier(1, std::addressof(barrier));

}
	


DefaultBuffer::DefaultBuffer(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, const fs::path& path)
{
	if (fs::exists(path) == false) {
		Console.WarningLog("{} 파일을 찾을 수 없습니다. 기본 텍스쳐가 생성됩니다.",path.string());
		DefaultBuffer::LoadDefaultTexture(device, commandList);
	}
	else {
		const std::wstring& ext = path.extension();
		
		std::unique_ptr<uint8_t[]> data{ nullptr };
		if (ext == L".DDS" or ext == L".dds") {
			std::vector<D3D12_SUBRESOURCE_DATA> subresources{};
			CheckHR(DirectX::LoadDDSTextureFromFile(device.Get(), path.c_str(), mDefaultBuffer.GetAddressOf(),data,subresources));



			auto uploadHeapsize = GetRequiredIntermediateSize(mDefaultBuffer.Get(), 0, static_cast<UINT>(subresources.size()));


			CD3DX12_RESOURCE_DESC bufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(uploadHeapsize)) };
			CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_UPLOAD };

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

			CD3DX12_RESOURCE_BARRIER barrier{ CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST) };

			commandList->ResourceBarrier(1, std::addressof(barrier));

			UpdateSubresources(commandList.Get(), mDefaultBuffer.Get(), mUploadBuffer.Get(), 0, 0,static_cast<UINT>(subresources.size()),subresources.data());

			barrier = { CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_GENERIC_READ) };

			commandList->ResourceBarrier(1, std::addressof(barrier));
		}
		else {
			D3D12_SUBRESOURCE_DATA subresource{};
			DirectX::LoadWICTextureFromFile(device.Get(), path.c_str(), mDefaultBuffer.GetAddressOf(),data,subresource);
			
			auto uploadHeapsize = GetRequiredIntermediateSize(mDefaultBuffer.Get(), 0, 1);
			CD3DX12_RESOURCE_DESC bufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(uploadHeapsize)) };
			CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_UPLOAD };

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


			CD3DX12_RESOURCE_BARRIER barrier{ CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST) };

			commandList->ResourceBarrier(1, std::addressof(barrier));

			UpdateSubresources(commandList.Get(), mDefaultBuffer.Get(), mUploadBuffer.Get(), 0, 0, 1, std::addressof(subresource));

			barrier = { CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_GENERIC_READ) };

			commandList->ResourceBarrier(1, std::addressof(barrier));
		}
			 
		Console.InfoLog("{} 텍스쳐를 로드했습니다.", path.string());
	}
}

DefaultBuffer::~DefaultBuffer() {

}

DefaultBuffer::DefaultBuffer(DefaultBuffer&& other) noexcept {
	mDefaultBuffer = std::move(other.mDefaultBuffer);
	other.mDefaultBuffer.Reset();
}

DefaultBuffer& DefaultBuffer::operator=(DefaultBuffer&& other) noexcept {
	if (this != std::addressof(other)) {
		mDefaultBuffer = std::move(other.mDefaultBuffer);
		other.mDefaultBuffer.Reset();
	}
	return *this;
}

ComPtr<ID3D12Resource> DefaultBuffer::GetBuffer() const
{
	return mDefaultBuffer;
}



void DefaultBuffer::LoadDefaultTexture(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	const std::wstring& path = L"Resources/textures/DefaultTexture.png";

	D3D12_SUBRESOURCE_DATA subresource{};
	std::unique_ptr<uint8_t[]> data{ nullptr };

	DirectX::LoadWICTextureFromFile(device.Get(), path.c_str(), mDefaultBuffer.GetAddressOf(), data, subresource);

	auto uploadHeapsize = GetRequiredIntermediateSize(mDefaultBuffer.Get(), 0, 1);
	CD3DX12_RESOURCE_DESC bufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(uploadHeapsize)) };
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_UPLOAD };

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


	CD3DX12_RESOURCE_BARRIER barrier{ CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_DEST) };

	commandList->ResourceBarrier(1, std::addressof(barrier));

	UpdateSubresources(commandList.Get(), mDefaultBuffer.Get(), mUploadBuffer.Get(), 0, 0, 1, std::addressof(subresource));

	barrier = { CD3DX12_RESOURCE_BARRIER::Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_GENERIC_READ) };

	commandList->ResourceBarrier(1, std::addressof(barrier));

}

