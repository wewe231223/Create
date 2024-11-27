#include "pch.h"
#include "ResourceManager.h"
#include "buffer/DefaultBuffer.h"
#include "resource/Mesh.h"
#include "resource/Model.h"
#include "resource/Shader.h"
#include "ui/Console.h"
#include "scene/BoundingBoxRenderer.h"


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
	return ( mModels.empty() and mNewModels.empty() );
} 

void ResourceManager::ModelContainer::Insert(const std::string& model, std::shared_ptr<Model>&& newModel)
{
	mModelMap[model] = newModel;

	auto nIt = std::find_if(mNewModels.begin(), mNewModels.end(), [newModel](const auto& pair) {
		return pair.first == newModel->GetShaderID();
		});
	// 이미 등록된 셰이더이면 
	if (nIt != mNewModels.end()) {
		nIt->second.emplace_back(newModel);
		return;
	}  
	// 아니라면 
	nIt = std::upper_bound(mNewModels.begin(), mNewModels.end(), newModel->GetShaderID(),
		[](auto& a, auto& b) {
			return a < b.first;
		}
	);

	mNewModels.emplace(nIt, newModel->GetShaderID(), std::vector<std::shared_ptr<Model>>{ newModel });
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

std::vector<std::pair<size_t, std::vector<std::shared_ptr<class Model>>>>::iterator ResourceManager::ModelContainer::begin()
{
	return mNewModels.begin();
}

std::vector<std::pair<size_t, std::vector<std::shared_ptr<class Model>>>>::iterator ResourceManager::ModelContainer::end()
{
	return mNewModels.end();
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
	mModelBoundingBoxRenderer = std::make_unique<BoundingBoxRenderer>(device);
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

D3D12_GPU_DESCRIPTOR_HANDLE ResourceManager::GetTexHandle()
{
	return mTexHeap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE ResourceManager::GetTexArrHandle()
{
	abort();
	return CD3DX12_GPU_DESCRIPTOR_HANDLE{};
}

D3D12_GPU_DESCRIPTOR_HANDLE ResourceManager::GetCubeTexHandle()
{
	abort();
	return D3D12_GPU_DESCRIPTOR_HANDLE();
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

void ResourceManager::ToggleRenderBB()
{
	if (mRenderBB) {
		mRenderBB = false;
	}
	else {
		mRenderBB = true;
	}
}

void ResourceManager::PrepareRender(ComPtr<ID3D12GraphicsCommandList>& commandList, D3D12_GPU_VIRTUAL_ADDRESS cameraBuffer)
{
#ifdef MODEL_CONT_CHECK_EMPTY 
	if (not mModelContainer->Empty()) {
		//std::shared_ptr<Model>& prev = *mModelContainer->begin();
		//prev->SetShader(commandList);
		//commandList->SetDescriptorHeaps(1, mTexHeap.GetAddressOf());
		//ResourceManager::SetGlobals(commandList);

		

		auto prev = mModelContainer->begin();
		(*prev).second.front()->SetShader(commandList);

		commandList->SetDescriptorHeaps(1, mTexHeap.GetAddressOf());
		ResourceManager::SetGlobals(commandList);
	}
#else 
	std::shared_ptr<Model>& prev = *mModelContainer->begin();
	prev->SetShader(commandList);
	commandList->SetDescriptorHeaps(1, mTexHeap.GetAddressOf());
	ResourceManager::SetGlobals(commandList);
#endif 
	commandList->SetGraphicsRootConstantBufferView(GRP_CameraConstants, cameraBuffer);
}

void ResourceManager::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
#ifdef MODEL_CONT_CHECK_EMPTY 
	if (not mModelContainer->Empty()) {
		auto prev = mModelContainer->begin();
		auto cur = mModelContainer->end();
		
		for (auto range = mModelContainer->begin(); range != mModelContainer->end(); ++range) {
			(*range).second.front()->SetShader(commandList);
			
			for (auto& model : range->second) {
				model->SetModelContext(commandList);
				model->Render(commandList);
				if(not mRenderBB) {
					model->EndRender();
				}
			}			
		}


		if (mRenderBB) {
			mModelBoundingBoxRenderer->SetShader(commandList);

			for (auto range = mModelContainer->begin(); range != mModelContainer->end(); ++range) {
				for (auto& model : range->second) {
					model->SetModelContext(commandList);
					mModelBoundingBoxRenderer->Render(commandList, model->GetInstanceCount());
					model->EndRender();
				}
			}

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




