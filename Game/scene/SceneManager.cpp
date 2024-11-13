#include "pch.h"
#include "Game/scene/SceneManager.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance{};
	return instance;
}

void SceneManager::ProcessPackets(std::shared_ptr<NetworkManager>& networkManager)
{
	mCurrentScene->ProcessPackets(networkManager);
}

void SceneManager::Send(std::shared_ptr<NetworkManager>& networkManager)
{
	mCurrentScene->Send(networkManager);
}

void SceneManager::Initialize(DxRenderer* renderer)
{
	mDxRenderer = renderer;
}

void SceneManager::Update()
{
	mCurrentScene->Update();
}

void SceneManager::CheckSceneChanged()
{
	if (mNextScene) {
		mCurrentScene.reset();
		mCurrentScene = mNextScene;
		mNextScene.reset();
		mDxRenderer->LoadScene(mCurrentScene);
	}
}

