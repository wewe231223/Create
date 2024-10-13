#include "pch.h"
#include "Game/gameobject/GameObject.h"


GameObject::GameObject()
{
	
}

GameObject::GameObject(const std::string& name)
{
}

GameObject::GameObject(std::shared_ptr<I3DRenderable> model)
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
	for (auto& child : mChildObjects) {
		child->SetMaterial(materials);
	}
}

void GameObject::SetChild(GameObject& child)
{
	mChildObjects.emplace_back(&child);
	mTransform.SetChild(&child.mTransform);
}

GameObject* GameObject::GetChild(UINT dfsIndex)
{
	if (dfsIndex == 0) {
		return this;
	}

	GameObject* ret = nullptr;
	UINT index = dfsIndex - 1;


	for (auto& child : mChildObjects) {
		ret = child->GetChildInternal(index);
		if (ret != nullptr) {
			break;
		}
	}


	return ret;
}

GameObject* GameObject::GetChildInternal(UINT& dfsIndex)
{
	if (dfsIndex == 0) {
		return this;
	}

	dfsIndex -= 1;

	GameObject* ret = nullptr;

	for (auto& child : mChildObjects) {
		ret = child->GetChildInternal(dfsIndex);

		if (ret != nullptr) {
			break;
		}
	}


	return ret;
}

Transform& GameObject::GetTransform()
{
	return mTransform;
}

void GameObject::Update()
{
	mTransform.CreateWorldMatrix();
	mContext.World = mTransform.GetWorldMatrix().Transpose();

	for (auto& child : mChildObjects) {
		child->Update();
	}
}

void GameObject::Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mModel->WriteContext(std::addressof(mContext), std::span(mMaterials) );

	for (auto& child : mChildObjects) {
		child->Render(camera, commandList);
	}
}

bool GameObject::IsInFrustum(std::shared_ptr<Camera> camera)
{
	return camera->IsInFrustum(mTransform.GetBB());
}

