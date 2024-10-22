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
std::shared_ptr<UIObject> healthBarBase;
std::shared_ptr<UIObject> healthBar;

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
	mCameraModes[CT_ThirdPersonCamera] = std::make_shared<TPPCameraMode>(mMainCamera, mPlayer->GetChild(1)->GetTransform(), DirectX::SimpleMath::Vector3(0.f, 0.5f, -3.f));


	mCurrentCameraMode = mCameraModes[CT_ThirdPersonCamera];
	mCurrentCameraMode->Enter();
}
static float yaw = 0.f;
static float HP = 0.f;

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


	mResourceManager->CreateModel<TexturedModel>("Cube", mResourceManager->GetShader("TexturedObjectShader"), TexturedModel::BasicShape::Cube);
	
	mResourceManager->CreateTexture("Bullet", "./Resources/textures/Bullet.png");
	Material mat{};
	mat.Textures[0] = mResourceManager->GetTexture("Bullet");
	mResourceManager->CreateMaterial("BulletMaterial", mat);

	mBullets.Initialize(mResourceManager->GetModel("Cube"), std::vector<MaterialIndex>{ mResourceManager->GetMaterial("BulletMaterial") } );
	mBullets.AssignValidateCallBack([](const std::shared_ptr<Bullet>& bullet) { return bullet->Validate(); });

	mTerrain->MakeObjectOnTerrain(mPlayer);
	mGameObjects.emplace_back(mPlayer);


	mUIRenderer->CreateUIImage("Menu", "./Resources/textures/menu.png");
	mUIRenderer->CreateUIImage("HealthBarBase", "./Resources/textures/HealthBarBase.png");
	mUIRenderer->CreateUIImage("HealthBar", "./Resources/textures/HealthBar.png");

	ui = std::make_shared<UIObject>(mUIRenderer, mUIRenderer->GetUIImage("Menu"));
	ui->GetUIRect().LTx = 0.f;
	ui->GetUIRect().LTy = 0.f;
	ui->GetUIRect().width = 1920.f;
	ui->GetUIRect().height = 1080.f;
	//Time.AddEvent(100ms, []() {ui->AdvanceSprite(); return true; });

	healthBarBase = std::make_shared<UIObject>(mUIRenderer, mUIRenderer->GetUIImage("HealthBarBase"));
	auto& uirect = healthBarBase->GetUIRect();
	uirect.LTx = 10.f;
	uirect.LTy = 10.f;
	uirect.width = 500.f;
	uirect.height = 50.f;


	healthBar = std::make_shared<UIObject>(mUIRenderer, mUIRenderer->GetUIImage("HealthBar"));
	auto& uirect1 = healthBar->GetUIRect();
	uirect1.LTx = 10.f;
	uirect1.LTy = 10.f;
	uirect1.width = HP * 5.f;
	uirect1.height = 50.f;



	Input.RegisterKeyDownCallBack(DirectX::Keyboard::Keys::Tab, 0, [this]() { ui->ToggleActiveState(); });
	Input.RegisterKeyReleaseCallBack(DirectX::Keyboard::Keys::Tab, 0, [this]() { ui->ToggleActiveState(); });
	ui->SetActiveState(false);



	int sign{ NrSampler.Sample() };

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::W, sign, [=]() { mPlayer->GetTransform().Translate(mPlayer->GetTransform().GetForward() * Time.GetSmoothDeltaTime<float>() * 10.f); });

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::S, sign, [=]() { mPlayer->GetTransform().Translate(mPlayer->GetTransform().GetForward() * -Time.GetSmoothDeltaTime<float>() * 10.f); });

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::Q, sign, [this]() { yaw += Time.GetSmoothDeltaTime<float>(); });

	Input.RegisterKeyPressCallBack(DirectX::Keyboard::Keys::E, sign, [this]() { yaw -= Time.GetSmoothDeltaTime<float>(); });




	GameScene::InitCameraMode();
	mResourceManager->ExecuteUpload(commandQueue);

}

// 트랜스폼 회전을 쿼터니언으로 하니까 존나 부조리하네 
// 회전 문제를 해결해야 할 때가 왔다. 
// 완전 누적 방식으로 하던지, 회전을 계층별로 나누던지, 쿼터니언을 포기하던지. 
void GameScene::Update()
{
	mTerrain->UpdateGameObjectAboveTerrain();

	// mGameObjects[49]->GetTransform().Translate({ 0.02f,0.f,0.f });

	mPlayer->GetTransform().Rotate(yaw, 0.f, 0.f);

	//if (Input.GetKeyboardState().W) {
	//	mPlayer->GetTransform().Translate(mPlayer->GetTransform().GetForward() * Time.GetSmoothDeltaTime<float>() * 10.f);
	//}

	//if (Input.GetKeyboardState().S) {
	//	mPlayer->GetTransform().Translate(mPlayer->GetTransform().GetForward() * -Time.GetSmoothDeltaTime<float>() * 10.f);
	//}

	//if (Input.GetKeyboardState().Q) {
	//	yaw += Time.GetSmoothDeltaTime<float>();
	//}

	//if (Input.GetKeyboardState().E) {
	//	yaw -= Time.GetSmoothDeltaTime<float>();
	//}

	


	static float tankhead = 0.f;

	if (Input.GetMouseState().rightButton) {
		tankhead += Input.GetDeltaMouseX() * Time.GetSmoothDeltaTime<float>() * 0.3f;
		tankhead = std::fmodf(tankhead, DirectX::XM_2PI);
	}
	mPlayer->GetChild(1)->GetTransform().Rotate(tankhead, 0.f, 0.f);

	if(Input.GetKeyboardTracker().IsKeyPressed(DirectX::Keyboard::Space)) {
		auto bullet = mBullets.Acquire();
		if (bullet != nullptr) {
			bullet->GetTransform().SetPosition(mPlayer->GetChild(1)->GetTransform().GetPosition());
			bullet->GetTransform().Scale({ 0.1f,0.1f,0.1f });
			bullet->Reset(mPlayer->GetChild(1)->GetTransform().GetForward());
		}
	}

	if (HP < 100.f) {
		HP += Time.GetSmoothDeltaTime<float>() * 5.f;
		auto& uirect = healthBar->GetUIRect();
		uirect.width = HP * 5.f;
	}
	yaw = std::fmodf(yaw, DirectX::XM_2PI);

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

	mBullets.Update();
	for (auto& bullet : mBullets) {
		bullet->Update();
	}

	mMainCamera->Update();
}

void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	for (auto& object : mGameObjects) {
		object->Render(mMainCamera, commandList);
	}

	for (auto& bullet : mBullets) {
		bullet->Render(mMainCamera, commandList);
	}

	mMainCamera->RenderSkyBox();

	mResourceManager->PrepareRender(commandList);
	mMainCamera->Render(commandList);
	mResourceManager->Render(commandList);


	ui->Render();
	healthBarBase->Render();
	healthBar->Render();
	mUIRenderer->Render(commandList);
}



Bullet::Bullet(std::shared_ptr<I3DRenderable> model, const std::vector<MaterialIndex>& materials)
	: GameObject(model, materials)
{
}

Bullet::~Bullet()
{
}

void Bullet::Reset(DirectX::SimpleMath::Vector3 dir)
{
	mDirection = dir;
	mTimeOut = 5.f;
}

bool Bullet::Validate() const 
{
	return mTimeOut > std::numeric_limits<float>::epsilon();
}

void Bullet::Update()
{
	GetTransform().Translate(mDirection * Time.GetDeltaTime<float>() * 100.f);
	mTimeOut -= Time.GetDeltaTime<float>();
	GameObject::Update();
}
