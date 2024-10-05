#pragma once 

class MeshRenderer : public RenderableComponentBase {
public:
	static constexpr size_t TypeIndex = 1;

	MeshRenderer(const ModelInfo& model);
	~MeshRenderer();
	void SetMaterial(const std::vector<MaterialIndex>& materials);
	void SetCullState(bool state);

	void Render(GameObject* object, ComPtr<ID3D12GraphicsCommandList>& commandList) override;
private:
	std::shared_ptr<IRendererEntity> mModel{ nullptr };
	std::vector<MaterialIndex> mMaterials{};

	bool mCullState{ false };
	ModelContext mContext{};
};