#pragma once 

// 1. 셰이더 만들기
// 2. 모델 렌더링 사이에 끼워넣기 
class BoundingBoxRenderer {
	struct Buffer {
		ComPtr<ID3D12Resource> mBuffer{ nullptr };
		DirectX::SimpleMath::Vector3* mBufferPtr{ nullptr };
	};
public:
	BoundingBoxRenderer(ComPtr<ID3D12Device> device);
	~BoundingBoxRenderer();
public:
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	std::shared_ptr<IGraphicsShader> mShader{ nullptr };
	std::vector<DirectX::SimpleMath::Vector3> mCenters{};
};