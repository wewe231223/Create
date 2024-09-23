#pragma once 






class FrameMemory {
public:
	FrameMemory(ComPtr<ID3D12Device>& device);

	void CheckCommandCompleted(ComPtr<ID3D12Fence>& fence);

	void Reset();

	void SetFenceValue(UINT64 value);

	ComPtr<ID3D12CommandAllocator> GetAllocator() const;
private:
	ComPtr<ID3D12CommandAllocator> mCommandAllocator{ nullptr };
	UINT64 mFenceValue{ 0 };
};