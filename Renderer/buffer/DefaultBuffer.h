#pragma once 
 
class DefaultBuffer {
public:
	DefaultBuffer(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, size_t size);
	~DefaultBuffer();

	DefaultBuffer(const DefaultBuffer& other) = delete;
	DefaultBuffer& operator=(const DefaultBuffer& other) = delete;


	DefaultBuffer(DefaultBuffer&& other) noexcept;
	DefaultBuffer& operator=(DefaultBuffer&& other) noexcept;

private:
	ComPtr<ID3D12Resource> mUploadBuffer{ nullptr };
	ComPtr<ID3D12Resource> mDefaultBuffer{ nullptr };
public:
	ComPtr<ID3D12Resource> GetBuffer() const;
};