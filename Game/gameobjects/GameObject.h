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
Update()	                            매 프레임마다 호출
LateUpdate()	                        모든 Update가 호출된 후 호출
OnEnable()	                            오브젝트 활성화 시 호출
OnDisable()	                            오브젝트 비활성화 시 호출
*/
class MonoBehavior {
public:
    MonoBehavior();
    virtual ~MonoBehavior();

    virtual void Awake(class GameWorld*, GameObject*)       PURE;
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

// 게임 오브젝트를 상속하여 초기화를 모듈화하는 것은 가능하나, 이외의 specilaize 한 행위는 의도에 맞지 않음. 
class GameObject {
public:
    GameObject();
    virtual ~GameObject();
    
    // 다른 게임 오브젝트를 참조하고 싶은 게임 오브젝트들을 위해 존재함. 모든 게임 오브젝트가 로딩 된 후, Scene 이 그려지기 전에 호출됨. 
    void Awake(class GameWorld*);
    // 업데이트. 모든 업데이트에 GameWorld 가 있는 이유는 게임 오브젝트는 자신 외 다른 오브젝트들과도 상호작용 가능해야 하기 때문. 유니티의 [[serialized field]] 와 상통한 역할을 하기 위해 게임 월드를 필요로 하도록 만들었음
    void Update(class GameWorld*);
    // 카메라 같은 오브젝트를 위해 만들었음.
	void LateUpdate(class GameWorld*);
    // 사용자 호출을 목적으로 만들어진 함수가 아님. 
    void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
    
    // 이 함수는 초기화에 비활성화된 상태로 시작하고 싶은 게임 오브젝트를 위해 만들었음. MonoBehavior 의 OnDisable, OnEnable 을 호출하지 않음. 
    void SetActiveState(bool state);
    // 이 함수는 초기화에 쓰라고 만들어둔 것이 아님. 위의 함수를 사용해야함.  
    void SetActiveState(class GameWorld*, bool state);
    
    // 애들도 직렬화가 필요한 부분이다.. 
    // 정형적인 행위를 하는 Component 를 만드는 함수. Component 는 Render 에 관여하는 RenderableComponent 와 Update 가 필요한 UpdatableComponent 가 있고, 셋 중 하나에도 들지 않는 일반 Component 도 있음. 모든 컴포넌트는 TypeIndex 라는 static constexpr size_t 필드를 필요로 한다. 컴포넌트 디자인에 있어 명심할 것. 
    template<typename ComponentType, typename... Args> requires HasTypeIndex<ComponentType> && std::constructible_from<ComponentType, Args...>
    void AddComponent(Args&&... args);

    // Specialize 한 행위를 하는 MonoBehavior 을 만드는 함수. 최적화를 위해 각 GameObject 마다 MonoBehavior 은 하나로 제한함. 
    template<typename BehaviorType, typename... Args> 
	void MakeMonoBehavior(Args&&... args);

	template<typename ComponentType> requires HasTypeIndex<ComponentType>
    bool HasComponent() const;
    // 같은 GameObject 내 다른 컴포넌트 참조를 위해 이 함수를 만들었음. 
    template<typename ComponentType> requires HasTypeIndex<ComponentType> 
    ComponentType* GetComponent();
protected:
    std::vector<std::shared_ptr<RenderableComponentBase>>   mRenderableComponents{};
    std::vector<std::shared_ptr<UpdatableComponentBase>>    mUpdatableComponents{};
    std::vector<std::shared_ptr<ComponentBase>>             mComponents{};
	std::unique_ptr<MonoBehavior>                           mMonoBehavior{ nullptr };

	std::shared_ptr<Collider>   					        mCollider{ nullptr };          
    bool                                                    mActiveState{ true };
};

template<typename ComponentType, typename ...Args>
    requires HasTypeIndex<ComponentType> && std::constructible_from<ComponentType, Args...>
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
    mComponents.emplace(mComponents.begin() + typeIndex, std::reinterpret_pointer_cast<ComponentBase>(component));
}

template<typename BehaviorType, typename ...Args>
inline void GameObject::MakeMonoBehavior(Args && ...args)
{
	std::unique_ptr<MonoBehavior> behavior = std::make_unique<BehaviorType>(std::forward<Args>(args)...);
	mMonoBehavior = std::move(behavior);
}

template<typename ComponentType> requires HasTypeIndex<ComponentType>
inline bool GameObject::HasComponent() const 
{
    if (mComponents.size() <= ComponentType::TypeIndex) return false;
    if (mComponents[ComponentType::TypeIndex] == nullptr) return false;
    return true;
}

template<typename ComponentType> requires HasTypeIndex<ComponentType>
inline ComponentType* GameObject::GetComponent()
{    
    return reinterpret_cast<ComponentType*>((mComponents.begin() + ComponentType::TypeIndex)->get());
}
