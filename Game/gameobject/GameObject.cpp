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

GameObject::GameObject(std::shared_ptr<I3DRenderable> model, const MaterialIndex& material)
	: mModel(model)
{
	GameObject::SetMaterial(material);
	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());
}

GameObject::GameObject(std::shared_ptr<I3DRenderable> model, const std::vector<MaterialIndex>& materials)
	: mModel(model) 
{
	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());
	GameObject::SetMaterial(materials);
}

GameObject::~GameObject()
{
}

GameObject::operator bool() const
{
	return GameObject::GetActiveState();
}

bool GameObject::GetActiveState() const
{
	return mState;
}

void GameObject::SetMaterial(const std::vector<MaterialIndex>& materials)
{
	std::copy(materials.begin(), materials.end(), std::back_inserter(mMaterials));
	for (auto& child : mChildObjects) {
		child->SetMaterial(materials);
	}
}

void GameObject::SetMaterial(const MaterialIndex& material)
{
	mMaterials.clear();
	mMaterials.emplace_back(material);
	for (auto& child : mChildObjects) {
		child->SetMaterial(material);
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



std::shared_ptr<GameObject> GameObject::Clone()
{
	auto ptr = std::make_shared<GameObject>();

	ptr->mModel = mModel;
	ptr->mTransform = Transform{};
	ptr->mTransform = mTransform;

	for (auto& child : mChildObjects) {
		auto childClone = child->Clone();
		childClone->GetTransform().SetParent(&ptr->GetTransform());
		ptr->mChildObjects.emplace_back(std::move(childClone));
	}

	return ptr;
}

void GameObject::SetActive(bool state)
{
	if (mState == state) return;
	mState = state;
	if (mScript) {
		if (state) {
			mScript->OnEnable();
		}
		else {
			mScript->OnDisable();
		}
	}
}

void GameObject::Awake()
{
	if (mScript) {
		mScript->Awake();
	}
}

void GameObject::Update()
{
	if (!mState) return;

	if (mScript) {
		mScript->Update();
	}
}

void GameObject::UpdateShaderVariables()
{
	if (!mState) return;

	mTransform.CreateWorldMatrix();
	mContext.World = mTransform.GetWorldMatrix().Transpose();

	for (auto& child : mChildObjects) {
		child->UpdateShaderVariables();
	}
}

void GameObject::Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (!mState) return;

	mModel->WriteContext(std::addressof(mContext), std::span(mMaterials) );

	for (auto& child : mChildObjects) {
		child->Render(camera, commandList);
	}
}

bool GameObject::IsInFrustum(std::shared_ptr<Camera> camera)
{
	return camera->IsInFrustum(mTransform.GetBB());
}

Script::Script(std::shared_ptr<GameObject> owner)
	: mOwner(owner)
{
}

Script::~Script()
{
}

void Script::Awake()
{
}

void Script::Update()
{
}

void Script::OnEnable()
{
}

void Script::OnDisable()
{
}
