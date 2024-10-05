#pragma once 
class GameObject;

//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Component Base								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class ComponentBase {
public:
    ComponentBase();
    virtual ~ComponentBase();
};

class UpdatableComponentBase : public ComponentBase {
public:
    UpdatableComponentBase();
    virtual ~UpdatableComponentBase();

	virtual void Update(GameObject*) PURE;
};

class RenderableComponentBase : public ComponentBase {
public:
	RenderableComponentBase();
	virtual ~RenderableComponentBase();

	virtual void Render(GameObject* object, ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;
};

//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							   MonoBehavior								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////
/*
Awake()	                                인스턴스화 시 호출
Start()	                                활성화 시 첫 프레임 전에 호출
Update()	                            매 프레임마다 호출
LateUpdate()	                        모든 Update가 호출된 후 호출
OnEnable()	                            오브젝트 활성화 시 호출
OnDisable()	                            오브젝트 비활성화 시 호출
OnDestroy()	                            오브젝트 파괴 시 호출
*/
class MonoBehavior {
public:
    static constexpr size_t TypeIndex = 2;

    MonoBehavior();
    virtual ~MonoBehavior();

    virtual void Update(class GameWorld*,GameObject*)       PURE;
	virtual void LateUpdate(class GameWorld*, GameObject*)  PURE;
	virtual void OnEnable(class GameWorld*, GameObject*)    PURE;
	virtual void OnDisable(class GameWorld*, GameObject*)   PURE;
	virtual void OnDestroy(class GameWorld*, GameObject*)   PURE;
};



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							   GameObject    							//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

template<typename T>
concept HasTypeIndex = requires(T t) {
    { T::TypeIndex } -> std::convertible_to<size_t>;
};

class GameObject {
public:
    GameObject();
    ~GameObject();
    
    void Update(class GameWorld*);
	void LateUpdate(class GameWorld*);
    void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
    
    void SetActiveState(class GameWorld*, bool state);
    
    // 애들도 직렬화가 필요한 부분이다.. 
    template<typename ComponentType, typename... Args> requires HasTypeIndex<ComponentType> && std::movable<ComponentType> && std::constructible_from<ComponentType, Args...>
    void AddComponent(Args&&... args);

    template<typename BehaviorType, typename... Args> 
	void MakeMonoBehavior(Args&&... args);

    template<typename ComponentType> requires HasTypeIndex<ComponentType> 
    std::optional<ComponentType*> GetComponent();
private:
    std::vector<std::shared_ptr<RenderableComponentBase>>   mRenderableComponents{};
    std::vector<std::shared_ptr<UpdatableComponentBase>>    mUpdatableComponents{};
    std::vector<std::shared_ptr<ComponentBase>>             mComponents{};
	std::unique_ptr<MonoBehavior>                           mMonoBehavior{ nullptr };

    bool                                                    mActiveState{ true };
};

template<typename ComponentType, typename ...Args>
    requires HasTypeIndex<ComponentType> && std::movable<ComponentType> && std::constructible_from<ComponentType, Args...>
inline void GameObject::AddComponent(Args&& ...args)
{
	std::shared_ptr<ComponentType> component = std::make_shared<ComponentType>(std::forward<Args>(args)...);
	if constexpr (std::is_convertible_v<ComponentType*, UpdatableComponentBase*>) {
		mUpdatableComponents.emplace_back(component);
	}

	if constexpr (std::is_convertible_v<ComponentType*, RenderableComponentBase*>) {
		mRenderableComponents.emplace_back(component);
	}

    size_t typeIndex = ComponentType::TypeIndex;
    if (typeIndex >= mComponents.size()) {
        mComponents.resize(typeIndex + 1);
    }
	mComponents.emplace(mComponents.begin() + typeIndex,component);
}

template<typename BehaviorType, typename ...Args>
inline void GameObject::MakeMonoBehavior(Args && ...args)
{
	std::unique_ptr<MonoBehavior> behavior = std::make_unique<BehaviorType>(std::forward<Args>(args)...);
	mMonoBehavior = std::move(behavior);
}

template<typename ComponentType>
    requires HasTypeIndex<ComponentType>
inline std::optional<ComponentType*> GameObject::GetComponent()
{    
	if (mComponents.size() <= ComponentType::TypeIndex) return std::nullopt;
	if (mComponents[ComponentType::TypeIndex] == nullptr) return std::nullopt;
    
    return reinterpret_cast<ComponentType*>((mComponents.begin() + ComponentType::TypeIndex)->get());
}
