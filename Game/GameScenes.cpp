#include "pch.h"
#include "Game/GameScenes.h"
#include "Renderer/resource/TerrainImage.h"
#include "Game/Camera.h"

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
std::shared_ptr<IRendererEntity> re{ nullptr };

void GameScene::Update()
{

	ModelContext context{};
	
	for (auto i = 0; i < 10; ++i) {
		for (auto j = 0; j < 10; ++j) {
			context.World = DirectX::SimpleMath::Matrix::CreateTranslation(i * 200.f,0.f,j * 200.f);
			re->WriteContext(&context);
		}
	}


	//mCamera->GetTransform().Rotate(0.f, Time.GetDeltaTime<float>(), 0.f);

	if (Input.GetKeyboardState().W)
	{
		mCamera->GetTransform().Translate({ 0.f,1.f,0.f });
	}
}

void GameScene::Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mCamera = std::make_unique<Camera>(device, commandList);

	auto& CameraParam = mCamera->GetCameraParam();
	CameraParam.Aspect = 1920.f / 1080.f;
	CameraParam.FOV = DirectX::XMConvertToRadians(60.f);
	CameraParam.NearZ = 0.1f;
	CameraParam.FarZ = 3000.f;


	mCamera->UpdateStaticVariables();

	mCamera->GetTransform().Translate({ 0.f,100.f,0.f });

	mCamera->GetTransform().LookAt({0.f,0.f,0.f});

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
	
	re = mSceneResource->GetModel("TerrainModel");
	
	

	mSceneResource->UploadMaterial(device, commandList);
}

void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mSceneResource->PrepareRender(commandList);
	mCamera->SetVariables(commandList);
	mSceneResource->Render(commandList);
}
