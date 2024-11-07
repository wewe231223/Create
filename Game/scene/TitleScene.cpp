#include "pch.h"
#include "Game/scene/TitleScene.h"

TitleScene::TitleScene()
{
}

TitleScene::TitleScene(const std::string& name)
{
}

TitleScene::~TitleScene()
{
}

#ifndef STAND_ALONE
void TitleScene::ProcessPackets(std::shared_ptr<class NetworkManager>& networkManager)
{
}

void TitleScene::Send(std::shared_ptr<class NetworkManager>& networkManager)
{
}
#endif

void TitleScene::Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window)
{
	mWindow = window;
	mResourceManager = std::make_shared<ResourceManager>(device);
	mCanvas = std::make_shared<Canvas>(device, mResourceManager->GetLoadCommandList(), window);
	mCanvas->Load(); 

	mCanvas->CreateUIObject<Title>("Title",1920,1080);

	mResourceManager->ExecuteUpload(commandQueue);
}
#include "Game/scene/SceneManager.h"
#include "Game/scene/GameScene.h"
void TitleScene::Update()
{

	if (Input.GetKeyboardState().Enter)
	{
		SceneManager::GetInstance().ChangeScene<GameScene>();
	}

	mCanvas->Update();
}

void TitleScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mCanvas->Render(commandList);
}
