#include "pch.h"
#include "ResourceManager.h"
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

bool ResourceManager::ModelContainer::Empty() const noexcept
{
	return mModels.empty();
}

void ResourceManager::ModelContainer::Insert(const std::string& model, std::shared_ptr<Model>&& newModel)
{
	// std::lower_bound를 사용하여 새로운 모델이 들어갈 위치를 찾음
	auto it = std::upper_bound(mModels.begin(), mModels.end(), newModel,
		[](const std::shared_ptr<Model>& a, const std::shared_ptr<Model>& b) {
			return a->CompareShaderID(b);
		});

	// 적절한 위치에 삽입
	mModels.insert(it, newModel);
	mModelMap[model] = newModel;
}

std::shared_ptr<I3DRenderable> ResourceManager::ModelContainer::GetModel(const std::string& name)
{
	auto it = mModelMap.find(name);
	if (it == mModelMap.end()) {
		return nullptr;
	}
	it->second->AddRef();
	return it->second;
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
	
	mDevice = device;

	CheckHR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())));
	CheckHR(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mLoadCommandList.GetAddressOf())));
	CheckHR(mLoadCommandList->Close());
	CheckHR(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mTexHeap.GetAddressOf())));

	ResourceManager::Reset();
}

ResourceManager::~ResourceManager()
{
}

ComPtr<ID3D12Device> ResourceManager::GetDevice() const noexcept
{
	return mDevice;
}

ComPtr<ID3D12GraphicsCommandList>& ResourceManager::GetLoadCommandList() 
{
	return mLoadCommandList;
}

void ResourceManager::Reset()
{
	CheckHR(mCommandAllocator->Reset());
	CheckHR(mLoadCommandList->Reset(mCommandAllocator.Get(), nullptr));
}

void ResourceManager::CreateTexture(const std::string& name, const fs::path& path)
{
	auto newTexture = mTextures.emplace_back(std::make_unique<DefaultBuffer>(mDevice, mLoadCommandList, path))->GetBuffer();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = newTexture->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// TODO : 밉레벨은? 
	srvDesc.Texture2D.MipLevels = 1;

	CD3DX12_CPU_DESCRIPTOR_HANDLE texheapHandle{ mTexHeap->GetCPUDescriptorHandleForHeapStart() };
	texheapHandle.Offset(static_cast<UINT>(mTextures.size()), mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	mDevice->CreateShaderResourceView(newTexture.Get(), &srvDesc, texheapHandle);

	mTextureMap[name] = static_cast<TextureIndex>(mTextures.size() );
}

void ResourceManager::CreateMaterial(const std::string& name, const Material& material)
{
	mMaterials.push_back(material);
	mMaterialMap[name] = static_cast<MaterialIndex>(mMaterials.size() - 1);
}


void ResourceManager::UploadMaterial()
{
	if (mMaterials.empty()) {
		return;
	}

	auto size = sizeof(Material) * mMaterials.size();
	mMaterialBuffer = std::make_unique<DefaultBuffer>(mDevice, mLoadCommandList, mMaterials.data(),sizeof(Material) * mMaterials.size());
}

void ResourceManager::ExecuteUpload(ComPtr<ID3D12CommandQueue>& queue)
{
	ResourceManager::UploadMaterial();

	mLoadCommandList->Close();
	ID3D12CommandList* commandLists[] = { mLoadCommandList.Get() };
	queue->ExecuteCommandLists(1, commandLists);
}


void ResourceManager::SetGlobals(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	commandList->SetGraphicsRootShaderResourceView(GRP_MaterialSRV, mMaterialBuffer->GetBuffer()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(GRP_Texture, mTexHeap->GetGPUDescriptorHandleForHeapStart());
}

std::shared_ptr<IGraphicsShader> ResourceManager::GetShader(const std::string& name)
{
	return mShaderMap[name];
}

MaterialIndex ResourceManager::GetMaterial(const std::string& name)
{
	if (mMaterialMap.find(name) == mMaterialMap.end()) {
		return NULL_INDEX;
	}
	return mMaterialMap[name];
}

TextureIndex ResourceManager::GetTexture(const std::string& name)
{
	if (mTextureMap.find(name) == mTextureMap.end()) {
		return NULL_INDEX;
	}
	return mTextureMap[name];
}

std::shared_ptr<I3DRenderable> ResourceManager::GetModel(const std::string& name)
{
	return mModelContainer->GetModel(name);
}

void ResourceManager::PrepareRender(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
#ifdef MODEL_CONT_CHECK_EMPTY 
	if (not mModelContainer->Empty()) {
		std::shared_ptr<Model>& prev = *mModelContainer->begin();
		prev->SetShader(commandList);
		commandList->SetDescriptorHeaps(1, mTexHeap.GetAddressOf());
		ResourceManager::SetGlobals(commandList);
	}
#else 
	std::shared_ptr<Model>& prev = *mModelContainer->begin();
	prev->SetShader(commandList);
	commandList->SetDescriptorHeaps(1, mTexHeap.GetAddressOf());
	ResourceManager::SetGlobals(commandList);
#endif 
}

void ResourceManager::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
#ifdef MODEL_CONT_CHECK_EMPTY 
	if (not mModelContainer->Empty()) {
		auto prev = mModelContainer->begin();
		auto cur = mModelContainer->begin();
		if (cur == mModelContainer->end())
			return;

		cur->get()->Render(commandList);
		++cur;

		for (; cur != mModelContainer->end(); ++prev, ++cur) {
			if (cur->get()->CompareEqualShader(*prev)) {
				cur->get()->SetShader(commandList);
			}

			cur->get()->Render(commandList);
		}
	}
#else 
	auto prev = mModelContainer->begin();
	auto cur = mModelContainer->begin();
	if (cur == mModelContainer->end())
		return;

	cur->get()->Render(commandList);
	++cur;

	for (; cur != mModelContainer->end(); ++prev, ++cur) {
		if (cur->get()->CompareShader(*prev)) {
			cur->get()->SetShader(commandList);
			ResourceManager::SetGlobals(commandList);
		}

		cur->get()->Render(commandList);
	}

#endif 
}




