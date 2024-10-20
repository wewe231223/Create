#pragma once 

class GameObject {
public:
	GameObject();
	GameObject(const std::string& name);
	GameObject(std::shared_ptr<I3DRenderable> model);
	GameObject(std::shared_ptr<I3DRenderable> model, const std::vector<MaterialIndex>& materials);
	virtual ~GameObject();
public:
	// 서브메쉬 순서대로 된 벡터 입력할것... 
	virtual void SetMaterial(const std::vector<MaterialIndex>& materials);

	void SetChild(GameObject& child);
	GameObject* GetChild(UINT dfsIndex);

	Transform& GetTransform();
	// 할거 다하고 콜, 업데이트 다음 충돌 처리 다음 렌더링 
	virtual void Update();
	// 프러스텀 컬링은 별도로 콜할것...
	virtual void Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList);
protected:
	bool IsInFrustum(std::shared_ptr<Camera> camera);

	GameObject* GetChildInternal(UINT& dfsIndex);
protected:
	Transform mTransform{};
	
	ModelContext mContext{};
	std::vector<MaterialIndex> mMaterials{};
	std::shared_ptr<I3DRenderable> mModel{ nullptr };

	std::vector<std::shared_ptr<GameObject>> mChildObjects{};
};







