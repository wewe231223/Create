#pragma once 

class FrameMemory : public IFrameMemory {
public:
	FrameMemory(ComPtr<ID3D12Device>& device);
	void CheckCommandCompleted(ComPtr<ID3D12Fence>& fence);
	void Reset();
	void SetFenceValue(UINT64 value);
	ComPtr<ID3D12CommandAllocator> GetAllocator() const;

	virtual void Copy(void* source, EFrameMemoryDest dest, size_t size)			override;
	virtual void SetVariables(ComPtr<ID3D12GraphicsCommandList>& commandList)	override;
private:
	ComPtr<ID3D12CommandAllocator> mCommandAllocator{ nullptr };
	UINT64 mFenceValue{ 0 };

	ComPtr<ID3D12Resource> mObjectContextMemory{ nullptr };
	ComPtr<ID3D12Resource> mCameraContextMemory{ nullptr };

	void* mObjectContextMemoryPtr{ nullptr };
	void* mCameraContextMemoryPtr{ nullptr };
};
