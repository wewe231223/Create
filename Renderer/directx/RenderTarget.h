#pragma once 


// MRT 같은 여러개의 패스를 사용할 때를 고려해서 설계해보자 
class RenderTargetGroup {
	class RenderTarget {
	public:

	};

	class DepthStencil {

	};
public:
	RenderTargetGroup(ComPtr<ID3D12Device> device,UINT bufferCount);
	~RenderTargetGroup();

	// 하나만
	void SetRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList, UINT bufferIndex);
	// 전부 
	void SetRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList);

	// 하나만 
	void Clear(ComPtr<ID3D12GraphicsCommandList> commandList, UINT bufferIndex);
	// 전부 
	void Clear(ComPtr<ID3D12GraphicsCommandList> commandList);


private:
	ComPtr<ID3D12DescriptorHeap> mRtvHeap{ nullptr };
	ComPtr<ID3D12DescriptorHeap> mDsvHeap{ nullptr };

	
};
	