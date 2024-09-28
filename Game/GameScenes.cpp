#include "pch.h"
#include "Game/GameScenes.h"
#include "Renderer/resource/TerrainImage.h"

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

void GameScene::Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mSceneResource = std::make_unique<ResourceManager>(device);

	mSceneResource->CreateTexture(device, commandList, "TerrainTexture", L"./Resources/terrain/Grass.dds");
	mSceneResource->CreateMaterial(device, commandList,"TerrainMaterial", {});
	mSceneResource->CreateShader<TerrainShader>(device,"TerrainShader");
	mSceneResource->CreateModel<TerrainModel>(
		"TerrainModel",
		device, 
		commandList, 
		mSceneResource->GetShader("TerrainShader"), 
		std::make_shared<TerrainImage>("./Resources/terrain/HeightMap.raw"), 
		DirectX::SimpleMath::Vector3{1.f,1.f,1.f}, 
		mSceneResource->GetMaterial("TerrainMaterial")
	);

	mSceneResource->UploadMaterial(device, commandList);
}

void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{




	mSceneResource->Render(commandList);
}
