#include "pch.h"
#include "Game/GameObject.h"

ComponentBase::ComponentBase()
{
}

ComponentBase::~ComponentBase()
{
}


GameObject::GameObject()
{
	
}

void GameObject::Update()
{
	for (auto& component : mComponents) {
		component->Update(this);
	}
}
