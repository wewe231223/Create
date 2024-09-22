#pragma once 
 
class DefaultBuffer {
public:
	// 디폴트 버퍼를 위한 생성자입니다. 
	DefaultBuffer(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, size_t size);
	// 기본 텍스쳐 디폴트 버퍼를 위한 생성자입니다. 
	DefaultBuffer(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList,const fs::path& path);

	~DefaultBuffer();

	DefaultBuffer(const DefaultBuffer& other) = delete;
	DefaultBuffer& operator=(const DefaultBuffer& other) = delete;


	DefaultBuffer(DefaultBuffer&& other) noexcept;
	DefaultBuffer& operator=(DefaultBuffer&& other) noexcept;

	ComPtr<ID3D12Resource> GetBuffer() const;

	static void Upload(ComPtr<ID3D12CommandQueue>& commandQueue);
	static void Release();
private:
	void LoadDefaultTexture(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	ComPtr<ID3D12Resource> mDefaultBuffer{ nullptr };

	static std::unique_ptr<DirectX::ResourceUploadBatch> mUploader;
	static bool mUploaderClosed;
};