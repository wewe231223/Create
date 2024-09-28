#pragma once 

class GameObject {
public:
	// 비어있는 게임 오브젝트도 있을 수 있다. 카메라? 
	GameObject();
	~GameObject() = default;
	
	void Update();
	
	template<typename T> 
		requires std::derived_from<T, Component>
	std::optional<std::reference_wrapper<T>> GetComponent();
private:
	// 적법한 검사 절차 없이 임의 접근을 방지하기 위해 Private로 설정
	std::unique_ptr<Component[]> mComponents{ nullptr };
};

template<typename T> 
	requires std::derived_from<T, Component>
inline std::optional<std::reference_wrapper<T>> GameObject::GetComponent()
{
	static_assert(T::Type != EComponentType::NONE, "Define Component Type!!");

	if (mComponents[static_cast<size_t>(T::Type)].Type == T::Type)
	{
		return std::ref(static_cast<T&>(mComponents[static_cast<size_t>(T::Type)]));
	}
	else {
		return std::nullopt;
	}
}
