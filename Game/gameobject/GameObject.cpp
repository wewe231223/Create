#include "pch.h"
#include "Game/gameobject/GameObject.h"


GameObject::GameObject()
{
	
}

GameObject::GameObject(const std::string& name)
{
}

GameObject::GameObject(std::shared_ptr<IRendererEntity> model)
	: mModel(model)
{
	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());
}

GameObject::~GameObject()
{
}

void GameObject::SetMaterial(const std::vector<MaterialIndex>& materials)
{
	std::copy(materials.begin(), materials.end(), std::back_inserter(mMaterials));
}

Transform& GameObject::GetTransform()
{
	return mTransform;
}

void GameObject::Update()
{
	mTransform.CreateWorldMatrix();
	mContext.World = mTransform.GetWorldMatrix().Transpose();
}

void GameObject::Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mModel->WriteContext(std::addressof(mContext), std::span(mMaterials) );
}

bool GameObject::IsInFrustum(std::shared_ptr<Camera> camera)
{
	return camera->IsInFrustum(mTransform.GetBB());
}
