#include "pch.h"
#include "Game/GameObject.h"
#include "Game/GameWorld.h"

ComponentBase::ComponentBase()
{
}

ComponentBase::~ComponentBase()
{
}


UpdatableComponentBase::UpdatableComponentBase()
{
}

UpdatableComponentBase::~UpdatableComponentBase()
{
}

RenderableComponentBase::RenderableComponentBase()
{
}

RenderableComponentBase::~RenderableComponentBase()
{
}


MonoBehavior::MonoBehavior()
{
}

MonoBehavior::~MonoBehavior()
{
}


GameObject::GameObject()
{
	
}

GameObject::~GameObject()
{
}

void GameObject::Update(class GameWorld* world)
{
	if (mMonoBehavior) {
		mMonoBehavior->Update(world, this);
		for (auto& components : mUpdatableComponents) {
			components->Update(this);
		}
		mMonoBehavior->LateUpdate(world, this);
	}
	else {
		for (auto& components : mUpdatableComponents) {
			components->Update(this);
		}
	}
}

void GameObject::LateUpdate(GameWorld* world)
{
	if (mMonoBehavior) {
		mMonoBehavior->LateUpdate(world, this);
	}
}

void GameObject::SetActiveState(GameWorld* world, bool state)
{
	if (!mActiveState and state) {
		if (mMonoBehavior) {
			mMonoBehavior->OnEnable(world, this);
		}
		mActiveState = state;
	}
	else if (mActiveState and not state) {
		if (mMonoBehavior) {
			mMonoBehavior->OnDisable(world, this);
		}
		mActiveState = state;
	}
}

void GameObject::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	for (auto& components : mRenderableComponents) {
		components->Render(this, commandList);
	}
}

