#pragma once 

class DirectionalShadowCaster : public IShadowCasterBase {
public:
	DirectionalShadowCaster(ComPtr<ID3D12Device>& device,UINT64 shadowMapWidth,UINT shaderMapHeight,const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction);
	~DirectionalShadowCaster();
public:
	// 그림자 맵 클리어
	void Clear(ComPtr<ID3D12GraphicsCommandList>& commandList);
	// 그림자 맵을 깊이 버퍼로 설정
	void SetShadowMap(ComPtr<ID3D12GraphicsCommandList>& commandList);
	// 그림자 맵 리소스 Getter ( 2nd Pass 에 리소스로 바인딩하거나 그림자 맵의 내용을 그리는데 사용한다. ) 
	ComPtr<ID3D12Resource>& GetShadowMap();

	void UpdateCameraContext();
	CameraContext& GetCameraContext();
private:
	DirectX::SimpleMath::Vector3 mPosition{};
	DirectX::SimpleMath::Vector3 mDirection{};

	UINT mShadowMapWidth{};
	UINT mShadowMapHeight{};
	
	DirectX::SimpleMath::Matrix mViewMatrix{};
	DirectX::SimpleMath::Matrix mProjectionMatrix{};

	CameraContext mCameraContext{};
	// 그림자 맵
	ComPtr<ID3D12Resource> mShadowMap{ nullptr };
	ComPtr<ID3D12DescriptorHeap> mShadowMapHeap{ nullptr };
};