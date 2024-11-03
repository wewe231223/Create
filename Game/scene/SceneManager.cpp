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

void SceneManager::Initialize(DxRenderer* renderer)
{
	mDxRenderer = renderer;
}

void SceneManager::Update()
{
	SceneManager::CheckSceneChanged();
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

