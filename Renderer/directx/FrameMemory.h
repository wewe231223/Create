#pragma once 

class FrameMemory {
public:
	FrameMemory(ComPtr<ID3D12Device> device);

	ComPtr<ID3D12CommandAllocator> GetAllocator() const;
	void Reset();
private:
	ComPtr<ID3D12CommandAllocator> mCommandAllocator{ nullptr };
};