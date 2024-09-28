#include "pch.h"
#include "Game/GameObject.h"

GameObject::GameObject()
{
	mComponents = std::make_unique<Component[]>(static_cast<size_t>(EComponentType::END));
	
}
