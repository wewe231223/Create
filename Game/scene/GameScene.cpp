#include "pch.h"
#include "Renderer/resource/TerrainImage.h"
#include "Renderer/resource/BillBoard.h"
#include "Renderer/scene/LightManager.h"
#include "Game/scene/GameScene.h"
#include "Game/gameobject/GameObject.h"
#include "Game/gameobject/TerrainObject.h"
#include "Game/gameobject/CubeObject.h"
#include "Game/gameobject/TexturedObject.h"
#include "Game/gameobject/NormalTexturedObject.h"
#include "Game/utils/Math2D.h"
#include "Game/gameobject/UIObject.h"
#include "Game/scene/CameraMode.h"
#include "Game/scripts/SCRIPT_Player.h"
#include "Game/scripts/SCRIPT_Bullet.h"
#include "Game/subwindow/ChatWindow.h"
#include "Game/utils/RandomEngine.h"


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

void GameScene::LoadTextures()
{
	mResourceManager->CreateTexture("TankTextureRed", "./Resources/textures/CTF_TankFree_Base_Color_R.png");
	mResourceManager->CreateTexture("TankTextureGreen", "./Resources/textures/CTF_TankFree_Base_Color_G.png");
	mResourceManager->CreateTexture("TankTextureBlue", "./Resources/textures/CTF_TankFree_Base_Color_B.png");

	mResourceManager->CreateTexture("SkyBox_Front", "./Resources/textures/SkyBox_Front_0.dds");
	mResourceManager->CreateTexture("SkyBox_Back", "./Resources/textures/SkyBox_Back_0.dds");
	mResourceManager->CreateTexture("SkyBox_Top", "./Resources/textures/SkyBox_Top_0.dds");
	mResourceManager->CreateTexture("SkyBox_Bottom", "./Resources/textures/SkyBox_Bottom_0.dds");
	mResourceManager->CreateTexture("SkyBox_Left", "./Resources/textures/SkyBox_Left_0.dds");
	mResourceManager->CreateTexture("SkyBox_Right", "./Resources/textures/SkyBox_Right_0.dds");


	mResourceManager->CreateTexture("DarkSkyBox_Front", "./Resources/textures/DarkStorm4K_front.png");
	mResourceManager->CreateTexture("DarkSkyBox_Back", "./Resources/textures/DarkStorm4K_back.png");
	mResourceManager->CreateTexture("DarkSkyBox_Top", "./Resources/textures/DarkStorm4K_top.png");
	mResourceManager->CreateTexture("DarkSkyBox_Bottom", "./Resources/textures/DarkStorm4K_bottom.png");
	mResourceManager->CreateTexture("DarkSkyBox_Left", "./Resources/textures/DarkStorm4K_left.png");
	mResourceManager->CreateTexture("DarkSkyBox_Right", "./Resources/textures/DarkStorm4K_right.png");

	mResourceManager->CreateTexture("Cliff", "./Resources/textures/CaveCrystal1_Base_Diffuse.png");
	mResourceManager->CreateTexture("CliffNormal", "./Resources/textures/CaveCrystal1_Base_Normal.png");

	mResourceManager->CreateTexture("Bullet", "./Resources/textures/Bullet.png");

	mResourceManager->CreateTexture("Grass", "./Resources/textures/Grass01.png");
	mResourceManager->CreateTexture("Tree", "./Resources/textures/Tree.png");
}

void GameScene::CreateMaterials()
{
	mResourceManager->CreateMaterial("SkyBoxFrontMaterial", { mResourceManager->GetTexture("SkyBox_Front") });
	mResourceManager->CreateMaterial("SkyBoxBackMaterial", { mResourceManager->GetTexture("SkyBox_Back") });
	mResourceManager->CreateMaterial("SkyBoxTopMaterial", { mResourceManager->GetTexture("SkyBox_Top") });
	mResourceManager->CreateMaterial("SkyBoxBottomMaterial", { mResourceManager->GetTexture("SkyBox_Bottom") });
	mResourceManager->CreateMaterial("SkyBoxLeftMaterial", { mResourceManager->GetTexture("SkyBox_Left") });
	mResourceManager->CreateMaterial("SkyBoxRightMaterial", { mResourceManager->GetTexture("SkyBox_Right") });

	mResourceManager->CreateMaterial("CliffMaterial", { mResourceManager->GetTexture("Cliff"), mResourceManager->GetTexture("CliffNormal") });

	mResourceManager->CreateMaterial("BulletMaterial", { mResourceManager->GetTexture("Bullet") });

	mResourceManager->CreateMaterial("TankMaterialRed", { mResourceManager->GetTexture("TankTextureRed") });
	mResourceManager->CreateMaterial("TankMaterialGreen", { mResourceManager->GetTexture("TankTextureGreen") });
	mResourceManager->CreateMaterial("TankMaterialBlue", { mResourceManager->GetTexture("TankTextureBlue") });
}



void GameScene::InitCamera(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mMainCamera = std::make_shared<Camera>(device,mWindow);
	mMainCamera->GetTransform().SetPosition({ 0.f,100.f,0.f });
	mMainCamera->GetTransform().LookAt({ 10.f,10.f,10.f });
}

void GameScene::InitSkyBox(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mResourceManager->CreateModel<SkyBoxModel>("SkyBox", mResourceManager->GetShader("SkyBoxShader"));

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

void GameScene::InitUI(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mCanvas->Load();

	auto slider = mCanvas->CreateUIObject<Slider>("HealthBarBase", "HealthBar", POINT{ 10, 10 } , 500, 50);
	SCRIPT_Player::HPBar = slider;	

	auto slider2 = mCanvas->CreateUIObject<Slider>("HealthBarBase", "CoolDownBar", POINT{ 10, 70 }, 300, 20);
	SCRIPT_Player::CoolTimeBar = slider2;

	auto menu = mCanvas->CreateUIObject<Menu>("Menu",1920,1080);


	mChatWindow = std::make_unique<ChatWindow>();
}


void GameScene::InitCameraMode()
{
	mCameraModes[CT_FreeCamera] = std::make_shared<FreeCameraMode>(mMainCamera);
	mCameraModes[CT_ThirdPersonCamera] = std::make_shared<TPPCameraMode>(mMainCamera, mPlayer->GetChild(1)->GetTransform(), DirectX::SimpleMath::Vector3(0.f, 1.f, -3.f));


	mCurrentCameraMode = mCameraModes[CT_ThirdPersonCamera];
	mCurrentCameraMode->Enter();
}

void GameScene::Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window)
{

	mWindow = window;
	mResourceManager = std::make_shared<ResourceManager>(device);
	mCanvas = std::make_shared<Canvas>(device, mResourceManager->GetLoadCommandList(), window);
	mLightManager = std::make_shared<LightManager>(device);


	mResourceManager->CreateShader<TerrainShader>("TerrainShader");
	mResourceManager->CreateShader<TexturedObjectShader>("TexturedObjectShader");
	mResourceManager->CreateShader<NormalTexturedObjectShader>("NormalTexturedObjectShader");
	mResourceManager->CreateShader<SkyBoxShader>("SkyBoxShader");


	GameScene::LoadTextures();
	GameScene::CreateMaterials();
	GameScene::InitCamera(device, mResourceManager->GetLoadCommandList());
	GameScene::InitSkyBox(device, mResourceManager->GetLoadCommandList());
	GameScene::InitTerrain(device, mResourceManager->GetLoadCommandList());
	GameScene::InitUI(device, mResourceManager->GetLoadCommandList());

	mResourceManager->CreateModel<TexturedModel>("Cube", mResourceManager->GetShader("TexturedObjectShader"), TexturedModel::BasicShape::Cube);

	auto bulletInit = [](std::shared_ptr<I3DRenderable> model,MaterialIndex mat) {
		auto bullet = std::make_shared<GameObject>(model, mat);
		bullet->SetActive(false);
		bullet->MakeScript<SCRIPT_Bullet>();
		return bullet;
		};

	mBulletPool.Initialize(bulletInit,mResourceManager->GetModel("Cube"), mResourceManager->GetMaterial("BulletMaterial"));
	SCRIPT_Player::BulletPool = &mBulletPool;
	SCRIPT_Bullet::mTerrainCollider = mTerrain;
	SCRIPT_Player::Light = mLightManager->GetLight();

	mPlayer = std::make_shared<TexturedObject>(mResourceManager, "./Resources/bins/Tank.bin");
	SCRIPT_Bullet::mPlayer = mPlayer;

	mPlayer->MakeScript<SCRIPT_Player>(mResourceManager, PlayerColor_B);
	mTerrain->MakeObjectOnTerrain(mPlayer);
	mGameObjects.emplace_back(mPlayer);




	auto originCliff = std::make_shared<NormalTexturedObject>(mResourceManager, "./Resources/bins/Normal_Cliff.bin");

	for (auto i = 0; i < 130; ++i) {
		auto cliff = originCliff->Clone();
		cliff->GetTransform().SetPosition({ 0.f,0.f,i * 10.f });
		cliff->GetTransform().Rotate(DirectX::XMConvertToRadians(90.f), 0.f, 0.f);
		cliff->GetTransform().Scale({ 0.3f,0.3f,0.3f });
		cliff->SetMaterial({ mResourceManager->GetMaterial("CliffMaterial") });
		mTerrain->OnTerrain(cliff);
		mGameObjects.emplace_back(cliff);
	}


	for (auto i = 0; i < 130; ++i) {
		auto cliff = originCliff->Clone();
		cliff->GetTransform().SetPosition({ i* 10.f,0.f,1270.f });
		cliff->GetTransform().Scale({ 0.3f,0.3f,0.3f });
		cliff->SetMaterial({ mResourceManager->GetMaterial("CliffMaterial") });
		mTerrain->OnTerrain(cliff);
		mGameObjects.emplace_back(cliff);

	}


	for (auto i = 0; i < 130; ++i) {
		auto cliff = originCliff->Clone();
		cliff->GetTransform().SetPosition({ i * 10.f,0.f, 0.f });
		cliff->GetTransform().Scale({ 0.3f,0.3f,0.3f });
		cliff->SetMaterial({ mResourceManager->GetMaterial("CliffMaterial") });
		mTerrain->OnTerrain(cliff);
		mGameObjects.emplace_back(cliff);

	}


	for (auto i = 0; i < 130; ++i) {
		auto cliff = originCliff->Clone();
		cliff->GetTransform().SetPosition({ 1270.f,0.f,i * 10.f });
		cliff->GetTransform().Rotate(DirectX::XMConvertToRadians(90.f), 0.f, 0.f);
		cliff->GetTransform().Scale({ 0.3f,0.3f,0.3f });
		cliff->SetMaterial({ mResourceManager->GetMaterial("CliffMaterial") });
		mTerrain->OnTerrain(cliff);
		mGameObjects.emplace_back(cliff);

	}

	std::uniform_real_distribution<float> dist(0.f, 1270.f);
	for (auto i = 0; i < 100; ++i) {
		auto enemy = mPlayer->Clone();
		enemy->SetMaterial(mResourceManager->GetMaterial("TankMaterialRed"));
		enemy->GetTransform().Scale({ 0.1f,0.1f,0.1f });
		enemy->GetTransform().SetPosition({ dist(MersenneTwister),0.f,dist(MersenneTwister) });
		mTerrain->MakeObjectOnTerrain(enemy);
		mGameObjects.emplace_back(enemy);
		mEnemies.emplace_back(enemy);
	}

	
	mBillBoard = std::make_shared<BillBoard>(device,mResourceManager->GetLoadCommandList());

	

	std::vector<BillBoardVertex> vertices{ 25'000 };
	for (auto& vertex : vertices) {
		vertex.position = mTerrain->OnTerrain({ dist(MersenneTwister),10.f,dist(MersenneTwister) }) + DirectX::SimpleMath::Vector3{ 0.f,5.5f,0.f };
		vertex.halfWidth = 5.f;
		vertex.height = 5.f;
		vertex.texture = mResourceManager->GetTexture("Tree");
		vertex.up = mTerrain->GetNormal(vertex.position);
		vertex.spritable = true;
		vertex.spriteFrameInCol = 1;
		vertex.spriteFrameInRow = 16;
		vertex.spriteDuration = 16.f;

	}

	mBillBoard->MakeBillBoard(vertices);

	int	sign{ NrSampler.Sample() };

	Input.RegisterKeyDownCallBack(DirectX::Keyboard::P, sign, [this]() {

		if (mCurrentCameraMode->GetType() == CT_FreeCamera) {
			mCurrentCameraMode->ChangeCameraMode(mCameraModes[CT_ThirdPersonCamera]);
			mCurrentCameraMode = mCameraModes[CT_ThirdPersonCamera];
		}
		else {
			mCurrentCameraMode->ChangeCameraMode(mCameraModes[CT_FreeCamera]);
			mCurrentCameraMode = mCameraModes[CT_FreeCamera];
		}
		}
	);

	GameScene::InitCameraMode();
	mResourceManager->ExecuteUpload(commandQueue);
}


#ifndef STAND_ALONE
// 11-02 게임 씬에서 패킷 처리를 위한 코드 작성
void GameScene::ProcessPackets()
{
	if (nullptr == mNetworkManager) {
		return;
	}

	RecvBuffer recvBuffer;
	mNetworkManager->ReadFromRecvBuffer(recvBuffer);
	PacketChatting chatPacket;
	while (false == recvBuffer.Empty()) {
		if (recvBuffer.Read(reinterpret_cast<char*>(&chatPacket), sizeof(PacketChatting))) {
			std::string clientName{ std::format("Client {}", chatPacket.id) };
			mChatWindow->UpdateChatLog("{:^10}: {}\n", clientName,  chatPacket.chatBuffer );
		}
	}
	recvBuffer.Clean();
}
#endif

// 트랜스폼 회전을 쿼터니언으로 하니까 존나 부조리하네 
// 회전 문제를 해결해야 할 때가 왔다. 
// 완전 누적 방식으로 하던지, 회전을 계층별로 나누던지, 쿼터니언을 포기하던지. 
// 10.24 해결 ! 


void GameScene::Update()
{
	for (auto& object : mGameObjects) {
		object->Update();
	}


	for (auto& bullet : mBulletPool) {
		bullet->Update();
		
		for (auto& e : mEnemies) {
			if (e->GetTransform().GetBB().Intersects(bullet->GetTransform().GetBB())) {
				e->SetActive(false);
			}
		
		}

	}

	mCanvas->Update();
	mTerrain->UpdateGameObjectAboveTerrain();

	GameScene::UpdateShaderVariables();
}

void GameScene::UpdateShaderVariables()
{
	mCurrentCameraMode->Update();

	for (auto& object : mGameObjects) {
		object->UpdateShaderVariables();
	}

	mBulletPool.Update();
	for (auto& bullet : mBulletPool) {
		bullet->UpdateShaderVariables();
	}
	
	mMainCamera->Update();
}

void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	for (auto& object : mGameObjects) {
		object->Render(mMainCamera, commandList);
	}

	for (auto& bullet : mBulletPool) {
		bullet->Render(mMainCamera, commandList);
	}

	mMainCamera->RenderSkyBox();

	mResourceManager->PrepareRender(commandList,mMainCamera->GetCameraBufferAddress());
	mLightManager->SetLight(commandList);
	mResourceManager->Render(commandList);


	mBillBoard->Render(commandList, mResourceManager->GetTexHandle(), mMainCamera->GetCameraBufferAddress());

#ifndef STAND_ALONE
	mChatWindow->Render();
#endif 

	mCanvas->Render(commandList);
}





