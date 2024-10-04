#pragma once 
class GameObject;

class ComponentBase {
public:
    ComponentBase();
    virtual ~ComponentBase();

    // TBD : LateUpdate, FixedUpdate 
    virtual void Update(GameObject*) PURE;
};

template<typename T>
concept HasTypeIndex = requires(T t) {
    { T::TypeIndex } -> std::convertible_to<size_t>;
};

class GameObject {
public:
    GameObject();
    ~GameObject() = default;
    
    void Update();
    
    template<typename ComponentType, typename... Args>
        requires HasTypeIndex<ComponentType> && std::movable<ComponentType> && std::constructible_from<ComponentType, Args...>
    void AddComponent(Args&&... args);

    template<typename ComponentType> 
        requires HasTypeIndex<ComponentType> 
    ComponentType& GetComponent();
private:
    std::vector<std::unique_ptr<ComponentBase>> mComponents{};
};

template<typename ComponentType, typename ...Args>
    requires HasTypeIndex<ComponentType> && std::movable<ComponentType> && std::constructible_from<ComponentType, Args...>
inline void GameObject::AddComponent(Args&& ...args)
{
    size_t typeIndex = ComponentType::TypeIndex;
    if (typeIndex >= mComponents.size()) {
        mComponents.resize(typeIndex + 1);
    }
    
	mComponents.emplace(mComponents.begin() + typeIndex, std::make_unique<ComponentType>(std::forward<Args>(args)...));
}

template<typename ComponentType>
    requires HasTypeIndex<ComponentType>
inline ComponentType& GameObject::GetComponent()
{    
    return *(reinterpret_cast<ComponentType*>((mComponents.begin() + ComponentType::TypeIndex)->get()));
}
