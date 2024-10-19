#include "pch.h"
#include "Renderer/resource/TerrainImage.h"
#include "Game/scene/GameScene.h"
#include "Game/gameobject/GameObject.h"
#include "Game/gameobject/TerrainObject.h"
#include "Game/gameobject/CubeObject.h"
#include "Game/gameobject/BinObject.h"
#include "Game/utils/Math2D.h"
#include "Game/gameobject/UIObject.h"
#include "Game/scene/CameraMode.h"

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

std::shared_ptr<UIObject> ui;

/*
넷겜플 클라 원하는 점

빌보드가 아닌 나무 (이건 가능하면, 불가능하거나 힘들면 구현 X)
텍스트 출력 (콘솔X, 이거도 가능하면)

HP바
플레이어를 따라다니는 빌보드

UI ( 0 )

프리 카메라 ( 0 )
충돌처리 ( 어떤 충돌처리? )

터지는 효과 (파티클이든 아니든 상관 X, 이것도 가능하면)
실외 지형 경계가 안보이도록 (자연스럽게 보이게)
포탄 쏘기

*/

void GameScene::InitCamera(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mMainCamera = std::make_shared<Camera>(device,mWindow);
	mMainCamera->GetTransform().SetPosition({ 0.f,100.f,0.f });
	mMainCamera->GetTransform().LookAt({ 10.f,10.f,10.f });
}

void GameScene::InitSkyBox(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mResourceManager->CreateModel<SkyBoxModel>("SkyBox", mResourceManager->GetShader("SkyBoxShader"));

	mResourceManager->CreateTexture("SkyBox_Front", "./Resources/textures/SkyBox_Front_0.dds");
	mResourceManager->CreateTexture("SkyBox_Back", "./Resources/textures/SkyBox_Back_0.dds");
	mResourceManager->CreateTexture("SkyBox_Top", "./Resources/textures/SkyBox_Top_0.dds");
	mResourceManager->CreateTexture("SkyBox_Bottom", "./Resources/textures/SkyBox_Bottom_0.dds");
	mResourceManager->CreateTexture("SkyBox_Left", "./Resources/textures/SkyBox_Left_0.dds");
	mResourceManager->CreateTexture("SkyBox_Right", "./Resources/textures/SkyBox_Right_0.dds");

	Material skyBoxMaterial{};
	skyBoxMaterial.Textures[0] = mResourceManager->GetTexture("SkyBox_Front");

	mResourceManager->CreateMaterial("SkyBoxFrontMaterial", skyBoxMaterial);

	skyBoxMaterial.Textures[0] = mResourceManager->GetTexture("SkyBox_Back");
	mResourceManager->CreateMaterial("SkyBoxBackMaterial", skyBoxMaterial);

	skyBoxMaterial.Textures[0] = mResourceManager->GetTexture("SkyBox_Top");
	mResourceManager->CreateMaterial("SkyBoxTopMaterial", skyBoxMaterial);

	skyBoxMaterial.Textures[0] = mResourceManager->GetTexture("SkyBox_Bottom");
	mResourceManager->CreateMaterial("SkyBoxBottomMaterial", skyBoxMaterial);

	skyBoxMaterial.Textures[0] = mResourceManager->GetTexture("SkyBox_Left");
	mResourceManager->CreateMaterial("SkyBoxLeftMaterial", skyBoxMaterial);

	skyBoxMaterial.Textures[0] = mResourceManager->GetTexture("SkyBox_Right");
	mResourceManager->CreateMaterial("SkyBoxRightMaterial", skyBoxMaterial);


	mMainCamera->SetCameraSkyBox(std::make_shared<SkyBox>(mResourceManager->GetModel("SkyBox"), std::vector<MaterialIndex>{
		mResourceManager->GetMaterial("SkyBoxFrontMaterial"),
		mResourceManager->GetMaterial("SkyBoxBackMaterial"),
		mResourceManager->GetMaterial("SkyBoxTopMaterial"),
		mResourceManager->GetMaterial("SkyBoxBottomMaterial"),
		mResourceManager->GetMaterial("SkyBoxLeftMaterial"),
		mResourceManager->GetMaterial("SkyBoxRightMaterial")
		}
	));

}

void GameScene::InitTerrain(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	std::shared_ptr<TerrainImage> terrainImage = std::make_shared<TerrainImage>("./Resources/terrain/HeightMap.raw");

	mTerrain = std::make_shared<TerrainCollider>(terrainImage, DirectX::SimpleMath::Vector3(5.0f, 1.0f, 5.0f));
	mGameObjects.emplace_back(std::make_shared<TerrainObject>(mResourceManager, terrainImage, DirectX::SimpleMath::Vector3(5.0f, 1.0f, 5.0f)));
}

void GameScene::InitCameraMode()
{
	mCameraModes[CT_FreeCamera] = std::make_shared<FreeCameraMode>(mMainCamera);
	mCameraModes[CT_ThirdPersonCamera] = std::make_shared<TPPCameraMode>(mMainCamera, mPlayer->GetTransform(), DirectX::SimpleMath::Vector3(0.f, 2.f, -5.f));


	mCurrentCameraMode = mCameraModes[CT_ThirdPersonCamera];
	mCurrentCameraMode->Enter();
}

void GameScene::Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window)
{
	mWindow = window;
	mResourceManager = std::make_shared<ResourceManager>(device);
	mUIRenderer = std::make_shared<UIRenderer>(device, mResourceManager->GetLoadCommandList(),window);

	mResourceManager->CreateShader<TerrainShader>("TerrainShader");
	mResourceManager->CreateShader<TexturedObjectShader>("TexturedObjectShader");
	mResourceManager->CreateShader<SkyBoxShader>("SkyBoxShader");


	GameScene::InitCamera(device, mResourceManager->GetLoadCommandList());
	GameScene::InitSkyBox(device, mResourceManager->GetLoadCommandList());
	GameScene::InitTerrain(device, mResourceManager->GetLoadCommandList());


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


	
	mPlayer = std::make_shared<BinObject>(mResourceManager, "./Resources/bins/Tank.bin");
	mPlayer->SetMaterial({ mResourceManager->GetMaterial("TankMaterialGreen") });
	mPlayer->GetTransform().SetPosition({ 10.f,100.f,10.f });
	mPlayer->GetTransform().Scale({ 0.1f,0.1f,0.1f });

	int sign = NrSampler.Sample();
	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::W, sign, [this]() {
		mPlayer->GetTransform().Translate(mPlayer->GetTransform().GetForward() * Time.GetSmoothDeltaTime<float>() * 10.f);
		});

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::S, sign, [this]() {
		mPlayer->GetTransform().Translate(mPlayer->GetTransform().GetForward() * -Time.GetSmoothDeltaTime<float>() * 10.f);
		});


	


	mTerrain->MakeObjectOnTerrain(mPlayer);
	mGameObjects.emplace_back(mPlayer);


	mUIRenderer->CreateUIImage("TankTextureRed", "./Resources/textures/noBKG_KnightRun_strip.png");

	ui = std::make_shared<UIObject>(mUIRenderer, mUIRenderer->GetUIImage("TankTextureRed"), std::make_pair<UINT,UINT>( 768, 64 ), std::make_pair<UINT,UINT>( 96, 64 ));
	ui->GetUIRect().LTx = 100.f;
	ui->GetUIRect().LTy = 100.f;
	ui->GetUIRect().width = 500.f;
	ui->GetUIRect().height = 500.f;
	Time.AddEvent(100ms, []() {ui->AdvanceSprite(); return true; });



	GameScene::InitCameraMode();
	mResourceManager->ExecuteUpload(commandQueue);

}

// TODO : 위치가 이상하게 나온다, 이거만 해결하고 UI 로 넘어갈것. 
void GameScene::Update()
{

	// mGameObjects[49]->GetTransform().Translate({ 0.02f,0.f,0.f });

	mTerrain->UpdateGameObjectAboveTerrain();


	//mGameObjects[101]->GetTransform().SetPosition({ 100.f,200.f,100.f });

	


	//mMainCamera->GetTransform().SetRotate(DirectX::SimpleMath::Quaternion::Identity);
	//mMainCamera->GetTransform().LookAt(mGameObjects[101]->GetTransform());
	//mMainCamera->GetTransform().SetPosition({ pos + offset });
	//mTerrain->UpdateCameraAboveTerrain(mMainCamera);

	GameScene::UpdateShaderVariables();
}

void GameScene::UpdateShaderVariables()
{
	mCurrentCameraMode->Update();

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

	mMainCamera->RenderSkyBox();

	mResourceManager->PrepareRender(commandList);
	mMainCamera->Render(commandList);
	mResourceManager->Render(commandList);


	ui->Render();
	mUIRenderer->Render(commandList);
}


