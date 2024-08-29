#pragma once 
class RenderTarget {
public:
	RenderTarget();
	~RenderTarget();

	void Create(ID3D12Device* device);
	void Reset(ID3D12Device* device, UINT width, UINT height);
private:
private:
	ComPtr<ID3D12Resource> mPresentBuffer{ nullptr };
	ComPtr<ID3D12Resource> mCanvasBuffer{ nullptr };

	ComPtr<ID3D12DescriptorHeap> mRTVHeap{ nullptr };
	UINT mRTVIncreasement{ 0 };
};
