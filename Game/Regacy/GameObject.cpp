#include "pch.h"
#include "Game/gameobjects/GameObject.h"
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

void GameObject::Awake(GameWorld* world)
{
	if (mMonoBehavior) {
		mMonoBehavior->Awake(world, this);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	게임 오브젝트의 업데이트 흐름은 다음과 같음.																					//
//																															//
//	[ <-				위치 가변					->][ <-						위치 불변 						-> ]			//
//	Update(Component/MonoBehavior) -> Rigidbody -> LateUpdate(MonoBehavior) -> Camera FrustumCulling -> Render				//
//																															//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void GameObject::Update(class GameWorld* world)
{
	if (not mActiveState) return; 

	if (mMonoBehavior) {
		for (auto& components : mUpdatableComponents) {
			components->Update(this);
		}
		mMonoBehavior->Update(world, this);
	}
	else {
		for (auto& components : mUpdatableComponents) {
			components->Update(this);
		}
	}

	// 이 시점에서 카메라를 제외한 모든 오브젝트의 위치는 그 프레임에 대해 불변이다. 
	// 만약 위치를 변경하는 경우 프러스텀 컬링을 위해 월드 행렬을 비롯한 bounding box 를 다시 만들어야 한다. 
	if (GameObject::HasComponent<Transform>()) {
		GameObject::GetComponent<Transform>()->CreateWorldMatrix();
	}
}


void GameObject::LateUpdate(GameWorld* world)
{
	if (not mActiveState) return; 
	
	if (mMonoBehavior) {
		mMonoBehavior->LateUpdate(world, this);
	}
}

void GameObject::SetActiveState(bool state)
{
	mActiveState = state;
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

