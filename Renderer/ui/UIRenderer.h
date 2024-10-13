#pragma once 

// TODO :: 중앙화된 텍스쳐 관리자가 필요하다. 시간이 날 때 1순위로 리팩토링할것 

constexpr DirectX::XMFLOAT3X3 XMFLOAT3X3Identity{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
};

class UIRenderer : public I2DRenderable {
	struct UIContextBuffer {
		ComPtr<ID3D12Resource> mBuffer{ nullptr };
		ModelContext2D* mContext{};
	};
public:
	UIRenderer(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& loadCommandList);
	~UIRenderer();

public:
	void CreateUIImage(const std::string& name,const fs::path& path);
	TextureIndex GetUIImage(const std::string& name);

	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);

	virtual void WriteContext(ModelContext2D* data) override;
private:
	// 로드 이외에 사용하지 않는다. 
	ComPtr<ID3D12Device> mDevice{ nullptr };
	ComPtr<ID3D12GraphicsCommandList> mLoadCommandList{ nullptr };

	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	std::unique_ptr<class DefaultBuffer> mVertexBuffer{};

	D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};
	std::unique_ptr<class DefaultBuffer> mIndexBuffer{ nullptr };

	ComPtr<ID3D12DescriptorHeap> mTexHeap{ nullptr };

	std::vector<std::unique_ptr<class DefaultBuffer>> mImages{};

	std::array<UIContextBuffer, static_cast<size_t>(GC_FrameCount)> mContexts{};
	UINT mMemoryIndex{ 0 };
	UINT mInstanceCount{ 0 };

	std::unique_ptr<IGraphicsShader> mShader{ nullptr };

	std::vector<std::unique_ptr<class DefaultBuffer>> mTextures{};
	std::unordered_map<std::string, TextureIndex> mTextureMap{};
};


inline DirectX::XMFLOAT3X3 Create2DScaleMatrix(float scaleX, float scaleY) {
	return DirectX::XMFLOAT3X3{
		scaleX, 0.0f,   0.0f,
		0.0f,   scaleY, 0.0f,
		0.0f,   0.0f,   1.0f
	};
}