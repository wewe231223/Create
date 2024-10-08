#include "pch.h"
#include "MeshRenderer.h"
#include "Game/GameWorld.h"
MeshRenderer::MeshRenderer(const ModelInfo& modelInfo) 
	: mModel(modelInfo.model)
{
	assert(mModel != nullptr);
	mMaterials.resize(modelInfo.material.size());
	std::copy(modelInfo.material.begin(), modelInfo.material.end(), mMaterials.begin());
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::SetMaterial(const std::vector<MaterialIndex>& materials)
{
	mMaterials.resize(materials.size());
	std::copy(materials.begin(), materials.end(), mMaterials.begin());
}

void MeshRenderer::SetCullState(bool state)
{
	mCullState = state;
}

void MeshRenderer::Render(GameObject* object, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (not mCullState) {
		auto transform = object->GetComponent<Transform>();
		mContext.World = transform->GetWorldMatrix();

		mModel->WriteContext(std::addressof(mContext), mMaterials);
	}
}

