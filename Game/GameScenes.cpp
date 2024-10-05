#include "pch.h"
#include "Game/GameScenes.h"
#include "Renderer/resource/TerrainImage.h"
#include "Game/Camera.h"
#include "Game/utils/NonReplacementSampler.h"
#include "Game/GameWorld.h"

GameScene::GameScene()
	: Scene()
{
}


GameScene::GameScene(const std::string& name)
	: Scene(name)
{
}

GameScene::~GameScene()
{
}

void GameScene::Update()
{

}

void GameScene::Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window)
{
	mGameWorld = std::make_unique<GameWorld>(device);

	// 이 사이 씬에서 사용될 게임 오브젝트들을 채운다. 

	mGameWorld->GetSceneResource()->ExecuteUpload(commandQueue);
}

void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mGameWorld->PrepareRender(commandList);
	mGameWorld->Render(commandList);
}
