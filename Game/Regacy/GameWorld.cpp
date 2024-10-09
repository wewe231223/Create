#include "pch.h"
#include "Game/GameWorld.h"

GameWorld::GameWorld(std::shared_ptr<ResourceManager> resourcemgr)
	: mResource(resourcemgr)
{

}

GameWorld::~GameWorld()
{
}

ResourceManager* GameWorld::GetSceneResource()
{
	return mResource.get();
}

std::span<std::shared_ptr<GameObject>> GameWorld::GetGameObject(EGameObjectTag tag)
{
	return std::span(mGameObjectsbyTag[tag]);
}

void GameWorld::MakeGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	mGameObjects.emplace_back(gameObject);
}

void GameWorld::MakeGameObject(EGameObjectTag tag,const std::shared_ptr<GameObject>& gameObject)
{
	mGameObjectsbyTag[tag].emplace_back(gameObject);
	mGameObjects.emplace_back(mGameObjectsbyTag[tag].back());
}

void GameWorld::SetMainCamera(const std::shared_ptr<GameObject>& camera)
{
	mMainCamera = camera;
}

std::shared_ptr<GameObject> GameWorld::GetMainCamera()
{
	return mMainCamera;
}

void GameWorld::Awake()
{
	for (auto& gameobject : mGameObjects) {
		gameobject->Awake(this);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	게임 오브젝트의 업데이트 흐름은 다음과 같음.																					//
//																															//
//	[ <-				위치 가변					->][ <-						위치 불변 						-> ]			//
//	Update(Component/MonoBehavior) -> Rigidbody -> LateUpdate(MonoBehavior) -> Camera FrustumCulling -> Render				//
//																															//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GameWorld::Update()
{
	// Update 
	for (auto& gameobject : mGameObjects) {
		gameobject->Update(this);
	}
	
	// Rigidbody

	// LateUpdate
	for (auto& gameobject : mGameObjects) {
		gameobject->LateUpdate(this);
	}
	mMainCamera->Update(this);
	
	// FrustumCulling

}

void GameWorld::PrepareRender(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mResource->PrepareRender(commandList);
}

void GameWorld::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mMainCamera->Render(commandList);

	for (auto& gameobject : mGameObjects) {
		gameobject->Render(commandList);
	}
	mResource->Render(commandList);
}