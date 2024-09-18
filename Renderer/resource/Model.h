#pragma once
//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Model Basis									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class Model {
public:
	Model(std::shared_ptr<class GraphicsShaderBase> shader);
	~Model();

	void SetShader(ComPtr<ID3D12GraphicsCommandList> commandList);
	bool CompareShader(const std::shared_ptr<Model>& other) const noexcept;
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
protected:
	std::array<std::unique_ptr<class DefaultBuffer>,VertexAttrib_end>	mVertexBuffers{};
	std::unique_ptr<class DefaultBuffer>								mIndexBuffer{ nullptr };

	std::vector<D3D12_VERTEX_BUFFER_VIEW>								mVertexBufferViews{};
	D3D12_INDEX_BUFFER_VIEW												mIndexBufferView{};

	std::vector<std::unique_ptr<class Mesh>>							mMeshes{};
	std::shared_ptr<class GraphicsShaderBase>							mShader{ nullptr };

	MaterialIndex 														mMaterialIndex{ std::numeric_limits<MaterialIndex>::max() }; 
	VertexAttribute														mAttribute{ 0b0000'0000'0000'0000 };
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
	TerrainModel(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList,std::shared_ptr<class GraphicsShaderBase> terrainShader,std::shared_ptr<class TerrainImage> terrainImage, DirectX::SimpleMath::Vector3 scale,MaterialIndex matidx);
	~TerrainModel();
private:
	void Create(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	std::shared_ptr<class TerrainImage> mTerrainImage{ nullptr };
	DirectX::SimpleMath::Vector3 mScale{ 1.0f,1.0f,1.0f };
};