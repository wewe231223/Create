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
std::shared_ptr<IRendererEntity> re2{ nullptr };

void GameScene::Update()
{

	ModelContext context{};
	
	context.World = DirectX::SimpleMath::Matrix::CreateTranslation({ 0.f,0.f,0.f }).Transpose() ;
	re->WriteContext(&context);
	context.World = DirectX::SimpleMath::Matrix::CreateTranslation({ 0.f,300.f,0.f }).Transpose();
	re2->WriteContext(&context);
	
	if (Input.GetKeyboardState().E)
	{
		mCamera->GetTransform().Translate({ 0.f,0.1f,0.f });
	}
	if (Input.GetKeyboardState().Q)
	{
		mCamera->GetTransform().Translate({ 0.f,-0.1f,0.f });
	}
	if (Input.GetKeyboardState().W)
	{
		mCamera->GetTransform().Translate({ 0.f,0.f,0.1f });
	}
	if (Input.GetKeyboardState().S)
	{
		mCamera->GetTransform().Translate({ 0.f,0.f,-0.1f });
	}
	if (Input.GetKeyboardState().A)
	{
		mCamera->GetTransform().Translate({ -0.1f,0.f,0.f });
	}
	if (Input.GetKeyboardState().D)
	{
		mCamera->GetTransform().Translate({ 0.1f,0.f,0.f });
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

	mSceneResource->CreateTexture(device, commandList, "TerrainBaseTexture", L"./Resources/terrain/Base_Texture.dds");
	mSceneResource->CreateTexture(device, commandList, "TerrainDetailTexture", L"./Resources/terrain/Detail_Texture_7.dds");
	mSceneResource->CreateTexture(device, commandList, "aaa", L"./Resources/terrain/NormalMap.dds");

	Material mat{};
	mat.DiffuseTexIndex_1 = mSceneResource->GetTexture("TerrainBaseTexture");
	mat.DiffuseTexIndex_2 = mSceneResource->GetTexture("TerrainDetailTexture");

	mSceneResource->CreateMaterial(device, commandList,"TerrainMaterial", mat);
	
	mat.DiffuseTexIndex_2 = mSceneResource->GetTexture("aaa");
	mSceneResource->CreateMaterial(device, commandList, "TerrainMaterial2", mat);
	
	mSceneResource->CreateShader<TerrainShader>(device,"TerrainShader");
	mSceneResource->CreateModel<TerrainModel>(
		"TerrainModel",
		device, 
		commandList, 
		mSceneResource->GetShader("TerrainShader"), 
		std::make_shared<TerrainImage>("./Resources/terrain/HeightMap.raw"), 
		DirectX::SimpleMath::Vector3{1.f,1.f,1.f}
	);
	
	re = mSceneResource->GetModel("TerrainModel", { mSceneResource->GetMaterial("TerrainMaterial") });
	re2 = mSceneResource->GetModel("TerrainModel", { mSceneResource->GetMaterial("TerrainMaterial2") });
	

	mSceneResource->UploadMaterial(device, commandList);
}

void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mSceneResource->PrepareRender(commandList);
	mCamera->SetVariables(commandList);
	mSceneResource->Render(commandList);
}
