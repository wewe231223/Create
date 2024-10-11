#pragma once 

class GameObject {
public:
	GameObject();
	GameObject(const std::string& name);
	GameObject(std::shared_ptr<IRendererEntity> model);
	virtual ~GameObject();
public:
	// 서브메쉬 순서대로 된 벡터 입력할것... 
	void SetMaterial(const std::vector<MaterialIndex>& materials);

	Transform& GetTransform();
	// 할거 다하고 콜, 업데이트 다음 충돌 처리 다음 렌더링 
	virtual void Update();
	// 프러스텀 컬링은 별도로 콜할것...
	virtual void Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList);
protected:
	bool IsInFrustum(std::shared_ptr<Camera> camera);
protected:
	DirectX::SimpleMath::Matrix mLocalTransform{ DirectX::SimpleMath::Matrix::Identity };
	Transform mTransform{};
	
	ModelContext mContext{};
	std::vector<MaterialIndex> mMaterials{};
	std::shared_ptr<IRendererEntity> mModel{ nullptr };

};







