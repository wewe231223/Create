#include "pch.h"
#include "Renderer/resource/TerrainImage.h"
#include "Game/scene/GameScene.h"
#include "Game/gameobject/GameObject.h"
#include "Game/gameobject/TerrainObject.h"
#include "Game/gameobject/CubeObject.h"

#include "Game/gameobject/BinObject.h"

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
	mMainCamera->GetTransform().SetPosition({ 0.f,100.f,0.f });

	mMainCamera->GetTransform().LookAt({ 0.f,0.f,0.f });

	mResourceManager->CreateShader<TerrainShader>("TerrainShader");
	mResourceManager->CreateShader<TexturedObjectShader>("TexturedObjectShader");

	std::shared_ptr<TerrainImage> terrainImage = std::make_shared<TerrainImage>("./Resources/terrain/HeightMap.raw");

	mTerrain = std::make_shared<TerrainCollider>(terrainImage, DirectX::SimpleMath::Vector3(5.0f, 1.0f, 5.0f));
	mGameObjects.emplace_back(std::make_shared<TerrainObject>(mResourceManager, terrainImage, DirectX::SimpleMath::Vector3(5.0f, 1.0f, 5.0f)));


	mResourceManager->CreateTexture("TankTextureRed", "./Resources/textures/CTF_TankFree_Base_Color_R.png");
	mResourceManager->CreateTexture("TankTextureGreen", "./Resources/textures/CTF_TankFree_Base_Color_G.png");
	mResourceManager->CreateTexture("TankTextureBlue", "./Resources/textures/CTF_TankFree_Base_Color_B.png");

	Material tankMaterial;
	tankMaterial.Textures[0] = mResourceManager->GetTexture("TankTextureRed");

	mResourceManager->CreateMaterial("TankMaterialRed", tankMaterial);

	tankMaterial.Textures[0] = mResourceManager->GetTexture("TankTextureGreen");
	mResourceManager->CreateMaterial("TankMaterialGreen", tankMaterial);

	tankMaterial.Textures[0] = mResourceManager->GetTexture("TankTextureBlue");
	mResourceManager->CreateMaterial("TankMaterialBlue", tankMaterial);



	int mCallBackSign = NrSampler.Sample();


	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::W, mCallBackSign, [this]() {
		mMainCamera->GetTransform().Translate(mMainCamera->GetTransform().GetForward() * 0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::S, mCallBackSign, [this]() {
		mMainCamera->GetTransform().Translate(mMainCamera->GetTransform().GetForward() * -0.1f );
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::A, mCallBackSign, [this]() {
		mMainCamera->GetTransform().Translate(mMainCamera->GetTransform().GetRight() * 0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::D, mCallBackSign, [this]() {
		mMainCamera->GetTransform().Translate(mMainCamera->GetTransform().GetRight() * -0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::Q, mCallBackSign, [this]() {
		mMainCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Up * -0.1f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::E, mCallBackSign, [this]() {
		mMainCamera->GetTransform().Translate(DirectX::SimpleMath::Vector3::Up * 0.1f);
		});


	mResourceManager->CreateModel<TexturedModel>("Cube", mResourceManager->GetShader("TexturedObjectShader"), TexturedModel::Cube);


	for (auto i = 1; i <= 10; ++i) {
		for (auto j = 1; j <= 10; ++j) {
			auto cube = std::make_shared<CubeObject>(mResourceManager);
			cube->GetTransform().SetPosition({ i * 10.f, 0.f, j * 10.f });
			mTerrain->MakeObjectOnTerrain(cube);
			mGameObjects.emplace_back(cube);
		}
	}

	
	auto binObject = std::make_shared<BinObject>(mResourceManager, "./Resources/bins/Tank.bin");
	binObject->SetMaterial({ mResourceManager->GetMaterial("TankMaterialBlue") });
	binObject->GetTransform().SetPosition({ 200.f,0.f,10.f });
	binObject->GetTransform().Scale(0.3f, 0.5f, 0.5f);
	mTerrain->MakeObjectOnTerrain(binObject);
	mGameObjects.emplace_back(binObject);

	mResourceManager->ExecuteUpload(commandQueue);
}

void GameScene::Update()
{
	

	// mGameObjects[49]->GetTransform().Translate({ 0.02f,0.f,0.f });
	
	static float yaw = 0.f;
	yaw += 0.001f;
	mTerrain->UpdateGameObjectAboveTerrain();
	mGameObjects[49]->GetTransform().Rotate(yaw,0.f,0.f);

	auto child = mGameObjects[101]->GetChild(1);
	child->GetTransform().Rotate(yaw, 0.f, 0.f);
	
	auto pos = mGameObjects[101]->GetTransform().GetPosition();
	auto right = mGameObjects[101]->GetTransform().GetRight();
	auto up = mGameObjects[101]->GetTransform().GetUp();

	auto offset = right * 10.f + up * 5.f;
	

	mMainCamera->GetTransform().SetRotate(DirectX::SimpleMath::Quaternion::Identity);
	mMainCamera->GetTransform().LookAt(mGameObjects[101]->GetTransform());
	mMainCamera->GetTransform().SetPosition({ pos + offset });
	mTerrain->UpdateCameraAboveTerrain(mMainCamera);

	GameScene::UpdateShaderVariables();


}

void GameScene::UpdateShaderVariables()
{
	for (auto& object : mGameObjects) {
		object->Update();
	}

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

