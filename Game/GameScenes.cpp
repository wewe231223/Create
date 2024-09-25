#include "pch.h"
#include "Game/GameScenes.h"
#include "Renderer/resource/TerrainImage.h"
#include "Renderer/resource/Model.h"
#include "Renderer/resource/Mesh.h"
#include "Renderer/resource/Shader.h"

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


	nt = mSceneResource->GetModel("TerrainModel");
}
// Create a simple view projection matrix
DirectX::SimpleMath::Matrix CreateViewProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	DirectX::SimpleMath::Matrix view = DirectX::SimpleMath::Matrix::CreateLookAt(
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, -10.0f), // Camera position
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),  // Camera target
		DirectX::SimpleMath::Vector3::Up               // Up direction
	);

	DirectX::SimpleMath::Matrix projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		fov,          // Field of view
		aspectRatio,  // Aspect ratio
		nearPlane,    // Near plane distance
		farPlane      // Far plane distance
	);

	return view * projection;
}

// Usage example
float fov = DirectX::XMConvertToRadians(60.0f);   // Field of view in degrees
float aspectRatio = 16.0f / 9.0f;                 // Aspect ratio of the screen
float nearPlane = 0.1f;                            // Near plane distance
float farPlane = 1000.0f;                          // Far plane distance

DirectX::SimpleMath::Matrix viewProjectionMatrix = CreateViewProjectionMatrix(fov, aspectRatio, nearPlane, farPlane);




void GameScene::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	ModelContext context{};
	context.World = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3{ 0.f,0.f,0.f });
	

	auto vp = CreateViewProjectionMatrix(DirectX::XMConvertToRadians(60.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
	vp.Transpose();

	mSceneResource->PrepareRender(commandList);

	commandList->SetGraphicsRoot32BitConstants(GRP_CameraConstants, 16, &vp, 0);



	nt->WriteContext(&context);
	mSceneResource->Render(commandList);
}
