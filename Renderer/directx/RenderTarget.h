#pragma once 


// MRT 같은 여러개의 패스를 사용할 때를 고려해서 설계해보자 
class RenderTargetGroup {
public:
	/// <summary>
	/// 이 함수는 매개변수로 받아온 rts, ds 에 대해 std::move 를 호출합니다. 
	/// 
	/// </summary>
	/// <param name="device"></param>
	/// <param name="rts"></param>
	/// <param name="ds"></param>
	RenderTargetGroup(ComPtr<ID3D12Device> device,std::vector<ComPtr<ID3D12Resource>>& rts,ComPtr<ID3D12Resource>& ds);
	~RenderTargetGroup();

	// 하나만 
	void Reset(UINT index);
	// 전부 
	void Reset();

	// 하나만
	void SetRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList, UINT index);
	// 전부 
	void SetRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList);

	// 하나만 
	void Clear(ComPtr<ID3D12GraphicsCommandList> commandList, UINT index);
	// 전부 
	void Clear(ComPtr<ID3D12GraphicsCommandList> commandList);


private:
	ComPtr<ID3D12DescriptorHeap>		mRtvHeap{ nullptr };
	ComPtr<ID3D12DescriptorHeap>		mDsvHeap{ nullptr };

	std::vector<ComPtr<ID3D12Resource>> mRenderTargets{};
	ComPtr<ID3D12Resource>				mDepthStencil{ nullptr };
};
	