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
std::shared_ptr<IRendererEntity> ree{ nullptr };


void GameScene::Update()
{

	ModelContext context{};

	context.World = DirectX::SimpleMath::Matrix::CreateTranslation({ 0.f,0.f,0.f }).Transpose();
	MaterialIndex mi[]{ mSceneResource->GetMaterial("TerrainMaterial") };
	ree->WriteContext(&context, std::span(mi));


	context.World = DirectX::SimpleMath::Matrix::CreateTranslation({ 0.f,5.f,0.f }).Transpose();
	mi[0] = mSceneResource->GetMaterial("aa");
	re->WriteContext(&context, std::span(mi));


	mCamera->GetTransform().LookAt({ 0.f,0.f,0.f });
	
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

void GameScene::Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList,std::shared_ptr<Window> window)
{
	mCamera = std::make_unique<Camera>(device, commandList, window);

	mCamera->UpdateStaticVariables();

	mCamera->GetTransform().Translate({ 0.f,100.f,0.f });

	mCamera->GetTransform().LookAt({0.f,0.f,0.f});

	mSceneResource = std::make_unique<ResourceManager>(device);

	mSceneResource->CreateTexture(device, commandList, "TerrainBaseTexture", L"./Resources/terrain/Base_Texture.dds");
	mSceneResource->CreateTexture(device, commandList, "TerrainDetailTexture", L"./Resources/terrain/Detail_Texture_7.dds");
	// mSceneResource->CreateTexture(device, commandList, "aa", L"./Resources/terrain/Detail_Texture_6.dds");



	Material mat{};
	mat.DiffuseTexIndex_1 = mSceneResource->GetTexture("TerrainBaseTexture");
	mat.DiffuseTexIndex_2 = mSceneResource->GetTexture("TerrainDetailTexture");

	mSceneResource->CreateMaterial(device, commandList,"TerrainMaterial", mat);
	mat.DiffuseTexIndex_1 = mSceneResource->GetTexture("TerrainDetailTexture");
	mSceneResource->CreateMaterial(device, commandList, "aa", mat);



	mSceneResource->CreateShader<TerrainShader>(device,"TerrainShader");
	mSceneResource->CreateShader<TexturedObjectShader>(device, "TexturedObjectShader");
	
	mSceneResource->CreateModel<TerrainModel>(
		"TerrainModel",
		device, 
		commandList, 
		mSceneResource->GetShader("TerrainShader"), 
		std::make_shared<TerrainImage>("./Resources/terrain/HeightMap.raw"), 
		DirectX::SimpleMath::Vector3{1.f,1.f,1.f}
	);

	mSceneResource->CreateModel<TexturedModel>(
		"TexturedModel",
		device,
		commandList,
		mSceneResource->GetShader("TexturedObjectShader"),
		TexturedModel::Cube
	);
	
	ree = mSceneResource->GetModel("TerrainModel");
	re = mSceneResource->GetModel("TexturedModel");

	mSceneResource->UploadMaterial(device, commandList);
}

void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mSceneResource->PrepareRender(commandList);
	mCamera->SetVariables(commandList);
	mSceneResource->Render(commandList);
}
