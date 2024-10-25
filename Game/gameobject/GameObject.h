#pragma once 


class Script {
public:
	Script(std::shared_ptr<class GameObject> owner);
	virtual ~Script();

public:
	virtual void Awake()		PURE;
	virtual void Update()		PURE;
	virtual void OnEnable()		PURE;
	virtual void OnDisable()	PURE;

protected:
	std::shared_ptr<class GameObject> mOwner{ nullptr };
};




class GameObject : public std::enable_shared_from_this<GameObject> {
public:
	GameObject();
	GameObject(const std::string& name);
	GameObject(std::shared_ptr<I3DRenderable> model);
	GameObject(std::shared_ptr<I3DRenderable> model, const std::vector<MaterialIndex>& materials);
	virtual ~GameObject();
public:
	// 서브메쉬 순서대로 된 벡터 입력할것... 
	virtual void SetMaterial(const std::vector<MaterialIndex>& materials);

	template<typename Ty,typename... Args> 
		requires std::derived_from<Ty, Script> && std::constructible_from<Ty,std::shared_ptr<GameObject>, Args...>
	void MakeScript(Args&&... args);

	void SetChild(GameObject& child);
	GameObject* GetChild(UINT dfsIndex);

	Transform& GetTransform();
	
	// 스크립트는 따로 복사할 것. 
	virtual std::shared_ptr<GameObject> Clone();

	void Awake();

	void Update();
	// 할거 다하고 콜, 업데이트 다음 충돌 처리 다음 렌더링 
	virtual void UpdateShaderVariables();
	// 프러스텀 컬링은 별도로 콜할것...
	virtual void Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList);
protected:
	bool IsInFrustum(std::shared_ptr<Camera> camera);

	GameObject* GetChildInternal(UINT& dfsIndex);
protected:
	Transform mTransform{};

	std::unique_ptr<Script> mScript{ nullptr };
	
	ModelContext mContext{};
	std::vector<MaterialIndex> mMaterials{};
	std::shared_ptr<I3DRenderable> mModel{ nullptr };

	std::vector<std::shared_ptr<GameObject>> mChildObjects{};
};


template<typename Ty, typename ...Args>
	requires std::derived_from<Ty, Script> && std::constructible_from<Ty,std::shared_ptr<GameObject>, Args...>
inline void GameObject::MakeScript(Args&& ...args)
{
	mScript = std::make_unique<Ty>(shared_from_this(), args...);
}
