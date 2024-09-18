

#include "pch.h"
#include "scene/ResourceManager.h"
#include "buffer/DefaultBuffer.h"
#include "resource/Mesh.h"
#include "resource/Model.h"
#include "resource/Shader.h"
#include "ui/Console.h"


//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//						Model Container									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

ResourceManager::ModelContainer::ModelContainer()
{
}

ResourceManager::ModelContainer::~ModelContainer()
{
}

void ResourceManager::ModelContainer::Insert(std::shared_ptr<Model>&& newModel)
{
	// std::lower_bound를 사용하여 새로운 모델이 들어갈 위치를 찾음
	auto it = std::upper_bound(mModels.begin(), mModels.end(), newModel,
		[](const std::shared_ptr<Model>& a, const std::shared_ptr<Model>& b) {
			return a->CompareShader(b);
		});

	// 적절한 위치에 삽입
	mModels.insert(it, newModel);
}

std::vector<std::shared_ptr<Model>>::iterator ResourceManager::ModelContainer::begin()
{
	return mModels.begin();
}

std::vector<std::shared_ptr<Model>>::iterator ResourceManager::ModelContainer::end()
{
	return mModels.end();
}



//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//						Scene Resource Manager							//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


ResourceManager::ResourceManager(ComPtr<ID3D12Device>& device)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = static_cast<UINT>(EGlobalConstants::GC_MaxTextureNumber);
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	mModelContainer = std::make_unique<ModelContainer>();
	
	CheckHR(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mTexHeap.GetAddressOf())));
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::CreateTexture(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, const std::string& name, const std::wstring& path)
{

}

void ResourceManager::CreateMaterial(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, const std::string& name, const Material& material)
{
	mMaterials.push_back(material);
	mMaterialMap[name] = static_cast<MaterialIndex>(mMaterials.size() - 1);
}


void ResourceManager::UploadMaterial(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (mMaterials.empty()) {
		return;
	}

	auto size = sizeof(Material) * mMaterials.size();
	mMaterialBuffer = std::make_unique<DefaultBuffer>(device,commandList,mMaterials.data(),sizeof(Material) * mMaterials.size());

}

void ResourceManager::SetGlobals(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	commandList->SetDescriptorHeaps(1, mTexHeap.GetAddressOf());
	commandList->SetGraphicsRootDescriptorTable(3, mTexHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->SetGraphicsRootShaderResourceView(2, mMaterialBuffer->GetBuffer()->GetGPUVirtualAddress());
}

std::shared_ptr<GraphicsShaderBase> ResourceManager::GetShader(const std::string& name)
{
	return mShaderMap[name];
}

MaterialIndex ResourceManager::GetMaterial(const std::string& name)
{
	return mMaterialMap[name];
}

TextureIndex ResourceManager::GetTexture(const std::string& name)
{
	return mTextureMap[name];
}

void ResourceManager::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	std::shared_ptr<Model>& prev = *mModelContainer->begin();
	prev->SetShader(commandList);
	ResourceManager::SetGlobals(commandList);

	for (auto& model : *mModelContainer) {
		model->Render(commandList);
		if (prev->CompareShader(model)) {
			model->SetShader(commandList);
			ResourceManager::SetGlobals(commandList);
			prev = model;
		}

	}

}



