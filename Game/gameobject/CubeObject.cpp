#include "pch.h"
#include "Game/gameobject/CubeObject.h"

CubeObject::CubeObject(std::shared_ptr<ResourceManager> resourceMgr)
{

	mModel = resourceMgr->GetModel("Cube");
	GameObject::SetMaterial({ resourceMgr->GetMaterial("TerrainMaterial") });

	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());
	mLocalTransform.Translation({ 0.f,1.f,0.f });
}

CubeObject::~CubeObject()
{
}


void CubeObject::Update()
{
	GameObject::Update();
}


void CubeObject::Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (GameObject::IsInFrustum(camera)) {
		GameObject::Render(camera, commandList);
	}
}
