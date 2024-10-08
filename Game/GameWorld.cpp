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

void GameWorld::MakeGameObject(std::shared_ptr<GameObject>& gameObject)
{
	mGameObjects.emplace_back(gameObject);
}

void GameWorld::MakeGameObject(EGameObjectTag tag, std::shared_ptr<GameObject>& gameObject)
{
	mGameObjectsbyTag[tag].emplace_back(gameObject);
	mGameObjects.emplace_back(mGameObjectsbyTag[tag].back());
}

void GameWorld::SetMainCamera(std::shared_ptr<GameObject>& camera)
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

void GameWorld::Update()
{
	for (auto& gameobject : mGameObjects) {
		gameobject->Update(this);
	}

	for (auto& gameobject : mGameObjects) {
		gameobject->LateUpdate(this);
	}
}

void GameWorld::PrepareRender(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mResource->PrepareRender(commandList);
}

void GameWorld::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	for (auto& gameobject : mGameObjects) {
		gameobject->Render(commandList);
	}
	mResource->Render(commandList);
}
