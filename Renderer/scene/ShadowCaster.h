#pragma once 

class ShaderCaster {
public:
	ShaderCaster(ComPtr<ID3D12Device>& device,UINT64 shadowMapWidth,UINT64 shaderMapHeight);
	~ShaderCaster();
public:
	void Clear(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void SetShadowMap(ComPtr<ID3D12GraphicsCommandList>& commandList);
	ComPtr<ID3D12Resource>& GetShadowMap() const;
private:
	// 그림자 맵
	ComPtr<ID3D12Resource> mShadowMap{ nullptr };
	ComPtr<ID3D12DescriptorHeap> mShadowMapHeap{ nullptr };
};