#pragma once 

// 1. 셰이더 만들기
// 2. 모델 렌더링 사이에 끼워넣기 

// 이미 바인딩 되어 있는 modelcontext 를 사용하여 렌더링한다.
class BoundingBoxRenderer {
public:
	BoundingBoxRenderer(ComPtr<ID3D12Device> device);
	~BoundingBoxRenderer();
public:
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList, UINT instanceCount);
private:
	std::shared_ptr<IGraphicsShader> mShader{ nullptr };

	ComPtr<ID3D12Resource> mDummyVertexBuffer{ nullptr };
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
};