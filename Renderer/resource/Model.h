#pragma once
//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Model Basis									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class Model : public I3DRenderable {
	struct ModelContextBuffer {
		ComPtr<ID3D12Resource> mBuffer{ nullptr };
		ModelContext* mBufferPtr{ nullptr };
	};
public:
	Model() = default;
	Model(ComPtr<ID3D12Device>& device,std::shared_ptr<IGraphicsShader> shader);
	~Model();

	virtual DirectX::BoundingOrientedBox GetBoundingBox() const override;
	virtual void WriteContext(ModelContext* data, const std::span<MaterialIndex>& materials) override;

	void AddRef();

	void SetShader(ComPtr<ID3D12GraphicsCommandList> commandList);
	size_t GetShaderID() const noexcept;
	bool CompareShaderID(const std::shared_ptr<Model>& other) const noexcept;
	bool CompareEqualShader(const std::shared_ptr<Model>& other) const noexcept;

	void SetModelContext(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void EndRender();
protected:
	void CreateBBFromMeshes(std::vector<DirectX::XMFLOAT3>& positions);
protected:
	std::array<std::unique_ptr<class DefaultBuffer>,VertexAttrib_end>		mVertexBuffers{};
	std::unique_ptr<class DefaultBuffer>									mIndexBuffer{ nullptr };

	std::vector<D3D12_VERTEX_BUFFER_VIEW>									mVertexBufferViews{};
	D3D12_INDEX_BUFFER_VIEW													mIndexBufferView{};

	std::vector<std::unique_ptr<class Mesh>>								mMeshes{};
	std::shared_ptr<IGraphicsShader>										mShader{ nullptr };

	std::array<ModelContextBuffer, static_cast<size_t>(GC_FrameCount)>		mModelContexts{};

	UINT																	mInstanceCount{ 0 };
	UINT																	mMemoryIndex{ 0 };

	VertexAttribute															mAttribute{ 0b0000'0000'0000'0000 };

	DirectX::BoundingOrientedBox											mBoundingBox{};
	UINT																	mRefCount{ 0 };
};



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Terrain										//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


class TerrainModel : public Model {
public:
	TerrainModel(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList,std::shared_ptr<IGraphicsShader> terrainShader,std::shared_ptr<class TerrainImage> terrainImage, DirectX::SimpleMath::Vector3 scale);
	~TerrainModel();
private:
	std::shared_ptr<class TerrainImage> mTerrainImage{ nullptr };
	DirectX::SimpleMath::Vector3 mScale{ 1.0f,1.0f,1.0f };
};





//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Textured Model								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class TexturedModel : public Model {
public:
	enum BasicShape {
		Cube
	};
public:
	TexturedModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<IGraphicsShader> shader, BasicShape shapeType);
	TexturedModel(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<IGraphicsShader> shader, std::vector<DirectX::XMFLOAT3>& positions, std::vector<DirectX::XMFLOAT3>& norms, std::vector<DirectX::XMFLOAT2>& uvs, std::vector<UINT>& indices);
	TexturedModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<IGraphicsShader> shader, std::vector<DirectX::XMFLOAT3>& positions, std::vector<DirectX::XMFLOAT3>& norms, std::vector<DirectX::XMFLOAT3>& tangents, std::vector<DirectX::XMFLOAT3>& biTangents , std::vector<DirectX::XMFLOAT2>& uvs, std::vector<UINT>& indices);
	~TexturedModel();
};


//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							SkyBox Model								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class SkyBoxModel : public Model {
public:
	SkyBoxModel(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList,std::shared_ptr<IGraphicsShader> shader);
	~SkyBoxModel();
};