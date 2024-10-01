#include "pch.h"
#include "buffer/DefaultBuffer.h"
#include "ui/Console.h"

std::unique_ptr<DirectX::ResourceUploadBatch> DefaultBuffer::mUploader	= nullptr;
bool DefaultBuffer::mUploaderClosed										= true;

DefaultBuffer::DefaultBuffer(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, size_t size) {
	if (mUploader == nullptr) {
		mUploader = std::make_unique<DirectX::ResourceUploadBatch>(device.Get());
	}

	if (mUploaderClosed) {
		mUploader->Begin();
		mUploaderClosed = false;
	}

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

	mUploader->Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData = data;
	subResourceData.RowPitch = size;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	mUploader->Upload(mDefaultBuffer.Get(), 0, &subResourceData, 1);
	mUploader->Transition(mDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
}

DefaultBuffer::DefaultBuffer(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, const fs::path& path)
{
	if (fs::exists(path) == false) {
		Console.WarningLog("{} 파일을 찾을 수 없습니다. 기본 텍스쳐가 생성됩니다.",path.string());
		DefaultBuffer::LoadDefaultTexture(device, commandList);
	}
	else {
		if (mUploader == nullptr) {
			mUploader = std::make_unique<DirectX::ResourceUploadBatch>(device.Get());
		}

		if (mUploaderClosed) {
			mUploader->Begin();
			mUploaderClosed = false;
		}

		const std::wstring& ext = path.extension();
		auto& UploaderRef = *mUploader.get();
		bool Loaded = false;

		if (ext == L".dds" or ext == L".DDS") {
			CheckHR(DirectX::CreateDDSTextureFromFile(device.Get(), UploaderRef, path.c_str(), mDefaultBuffer.GetAddressOf()));
			
		}
		else {
			CheckHR(DirectX::CreateWICTextureFromFile(device.Get(), UploaderRef, path.c_str(), mDefaultBuffer.GetAddressOf()));
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

void DefaultBuffer::Upload(ComPtr<ID3D12CommandQueue>& commandQueue)
{
	if (!mUploaderClosed) {
		mUploaderClosed = true;
		auto handle = mUploader->End(commandQueue.Get());
		handle.wait();
	}
}

void DefaultBuffer::Release()
{
	mUploader.reset();
}

void DefaultBuffer::LoadDefaultTexture(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (mUploader == nullptr) {
		mUploader = std::make_unique<DirectX::ResourceUploadBatch>(device.Get());
	}

	if (mUploaderClosed) {
		mUploader->Begin();
		mUploaderClosed = false;
	}

	const std::wstring& path = L"Resources/textures/DefaultTexture.png";
	 CheckHR(DirectX::CreateWICTextureFromFile(device.Get(), *mUploader.get(), path.c_str(), mDefaultBuffer.GetAddressOf()));
}

