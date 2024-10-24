#include "pch.h"
#include "Game/gameobject/BinObject.h"

// 내일 실제 데이터 넣어보고 렌더링 해보자
// 에셋 바이너라이저는 단순 정점 정보만 올리고 재질 정보는 별도로 바인딩 하는 것으로 한다. 

BinObject::BinObject(std::shared_ptr<ResourceManager>& resourceMgr, const fs::path& binPath)
{
	std::ifstream file(binPath, std::ios::binary);
	assert(file.is_open());
	size_t size{ 0 };

	std::string name;
	file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
	name.resize(size);
	file.read(name.data(), size);


	// 이미 등록된 모델이면 모델을 만드는 과정은 스킵. 
	if (resourceMgr->GetModel(name) != nullptr)
	{
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT2) * size, std::ios::cur);
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(UINT) * size, std::ios::cur);
	}
	else {
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> uvs;
		std::vector<UINT> indices;


		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		positions.resize(size);
		file.read(reinterpret_cast<char*>(positions.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		normals.resize(size);
		file.read(reinterpret_cast<char*>(normals.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		uvs.resize(size);
		file.read(reinterpret_cast<char*>(uvs.data()), sizeof(DirectX::XMFLOAT2) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		indices.resize(size);
		file.read(reinterpret_cast<char*>(indices.data()), sizeof(UINT) * size);

		resourceMgr->CreateModel<TexturedModel>(name, resourceMgr->GetShader("TexturedObjectShader"), positions, normals, uvs, indices);

	}



	mModel = resourceMgr->GetModel(name);

	DirectX::SimpleMath::Matrix localTransform{};
	file.read(reinterpret_cast<char*>(&localTransform), sizeof(DirectX::XMFLOAT4X4));
	mTransform.SetLocalTransform(localTransform);


	UINT childCount{ 0 };
	file.read(reinterpret_cast<char*>(&childCount), sizeof(UINT));


	for (UINT i = 0; i < childCount; ++i)
	{
		auto& child = mChildObjects.emplace_back(std::make_shared<BinObject>(resourceMgr, file));
		child->GetTransform().SetParent(&mTransform);
	}

	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());

}

BinObject::BinObject(std::shared_ptr<ResourceManager>& resourceMgr, std::ifstream& file)
{
	size_t size{ 0 };

	std::string name;
	file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
	name.resize(size);
	file.read(name.data(), size);


	// 이미 등록된 모델이면 모델을 만드는 과정은 스킵. 
	if (resourceMgr->GetModel(name) != nullptr)
	{
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT2) * size, std::ios::cur);
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(UINT) * size, std::ios::cur);
	}
	else {
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> uvs;
		std::vector<UINT> indices;


		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		positions.resize(size);
		file.read(reinterpret_cast<char*>(positions.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		normals.resize(size);
		file.read(reinterpret_cast<char*>(normals.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		uvs.resize(size);
		file.read(reinterpret_cast<char*>(uvs.data()), sizeof(DirectX::XMFLOAT2) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		indices.resize(size);
		file.read(reinterpret_cast<char*>(indices.data()), sizeof(UINT) * size);

		resourceMgr->CreateModel<TexturedModel>(name, resourceMgr->GetShader("TexturedObjectShader"), positions, normals, uvs, indices);

	}



	mModel = resourceMgr->GetModel(name);
	DirectX::SimpleMath::Matrix localTransform{};
	file.read(reinterpret_cast<char*>(&localTransform), sizeof(DirectX::XMFLOAT4X4));
	mTransform.SetLocalTransform(localTransform);

	UINT childCount{ 0 };
	file.read(reinterpret_cast<char*>(&childCount), sizeof(UINT));

	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());

	for (UINT i = 0; i < childCount; ++i)
	{
		auto& child = mChildObjects.emplace_back(std::make_shared<BinObject>(resourceMgr, file));
		mTransform.SetChild(&child->GetTransform());
		child->SetMaterial(mMaterials);
	}
}

BinObject::~BinObject()
{
}


void BinObject::UpdateShaderVariables()
{
	GameObject::UpdateShaderVariables();
}

void BinObject::Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (GameObject::IsInFrustum(camera)) {
		GameObject::Render(camera, commandList);
	}
}

