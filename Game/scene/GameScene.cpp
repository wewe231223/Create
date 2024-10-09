#include "pch.h"
#include "Renderer/resource/TerrainImage.h"
#include "Game/scene/GameScene.h"
#include "Game/gameobject/GameObject.h"
#include "Game/gameobject/TerrainObject.h"


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

void GameScene::Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window)
{
	mResourceManager = std::make_shared<ResourceManager>(device);
	mMainCamera = std::make_shared<Camera>(device,window);

	mResourceManager->CreateShader<TerrainShader>("TerrainShader");
	mResourceManager->CreateShader<TexturedObjectShader>("TexturedObjectShader");

	std::shared_ptr<TerrainImage> terrainImage = std::make_shared<TerrainImage>("./Resources/terrain/HeightMap.raw");

	mTerrain = std::make_shared<TerrainCollider>(terrainImage, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
	mGameObjects.emplace_back(std::make_shared<TerrainObject>(mResourceManager, terrainImage, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f)));

	mResourceManager->ExecuteUpload(commandQueue);
}

void GameScene::Update()
{
	for (auto& object : mGameObjects) {
		object->Update();
	}
	mTerrain->UpdateGameObjectAboveTerrain();

	mMainCamera->GetTransform().SetPosition({ 0.f,100.f,0.f });
	mMainCamera->GetTransform().LookAt({ 0.f,0.f,0.f });
	
	mMainCamera->Update();
}

void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{


	for (auto& object : mGameObjects) {
		object->Render(mMainCamera, commandList);
	}


	mResourceManager->PrepareRender(commandList);
	mMainCamera->Render(commandList);
	mResourceManager->Render(commandList);
}
