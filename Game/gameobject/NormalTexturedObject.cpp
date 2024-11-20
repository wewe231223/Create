#include "pch.h"
#include "Game/gameobject/NormalTexturedObject.h"

NormalTexturedObject::NormalTexturedObject(std::shared_ptr<ResourceManager>& resourceMgr, const fs::path& binPath)
{
	std::ifstream file(binPath, std::ios::binary);
	assert(file.is_open());
	size_t size{ 0 };

	std::string name;
	file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
	name.resize(size);
	file.read(name.data(), size);
	name = "Normaled_" + name;

	// 이미 등록된 모델이면 모델을 만드는 과정은 스킵. 
	if (resourceMgr->GetModel(name) != nullptr) {
		// positions 
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		// normals 
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		// tangents 
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		// bitangents
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		// uvs
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT2) * size, std::ios::cur);
		// indices 
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(UINT) * size, std::ios::cur);
	}
	else {
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT3> tangents;
		std::vector<DirectX::XMFLOAT3> bitangents;
		std::vector<DirectX::XMFLOAT2> uvs;
		std::vector<UINT> indices;


		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		positions.resize(size);
		file.read(reinterpret_cast<char*>(positions.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		normals.resize(size);
		file.read(reinterpret_cast<char*>(normals.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		tangents.resize(size);
		file.read(reinterpret_cast<char*>(tangents.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		bitangents.resize(size);
		file.read(reinterpret_cast<char*>(bitangents.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		uvs.resize(size);
		file.read(reinterpret_cast<char*>(uvs.data()), sizeof(DirectX::XMFLOAT2) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		indices.resize(size);
		file.read(reinterpret_cast<char*>(indices.data()), sizeof(UINT) * size);

		resourceMgr->CreateModel<TexturedModel>(name, resourceMgr->GetShader("NormalTexturedObjectShader"), positions, normals, tangents, bitangents, uvs, indices);

	}

	mModel = resourceMgr->GetModel(name);

	DirectX::SimpleMath::Matrix localTransform{};
	file.read(reinterpret_cast<char*>(&localTransform), sizeof(DirectX::XMFLOAT4X4));
	mTransform.SetLocalTransform(localTransform);


	UINT childCount{ 0 };
	file.read(reinterpret_cast<char*>(&childCount), sizeof(UINT));


	for (UINT i = 0; i < childCount; ++i) {
		auto& child = mChildObjects.emplace_back(std::make_shared<NormalTexturedObject>(resourceMgr, file));
		child->GetTransform().SetParent(&mTransform);
	}

	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());

}

NormalTexturedObject::NormalTexturedObject(std::shared_ptr<ResourceManager>& resourceMgr, std::ifstream& file)
{
	size_t size{ 0 };

	std::string name;
	file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
	name.resize(size);
	file.read(name.data(), size);
	name = "Normaled_" + name;

	// 이미 등록된 모델이면 모델을 만드는 과정은 스킵. 
	if (resourceMgr->GetModel(name) != nullptr) {
		// positions 
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		// normals 
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		// tangents 
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		// bitangents
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT3) * size, std::ios::cur);
		// uvs
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(DirectX::XMFLOAT2) * size, std::ios::cur);
		// indices 
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.seekg(sizeof(UINT) * size, std::ios::cur);
	}
	else {
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT3> tangents;
		std::vector<DirectX::XMFLOAT3> bitangents;
		std::vector<DirectX::XMFLOAT2> uvs;
		std::vector<UINT> indices;


		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		positions.resize(size);
		file.read(reinterpret_cast<char*>(positions.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		normals.resize(size);
		file.read(reinterpret_cast<char*>(normals.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		tangents.resize(size);
		file.read(reinterpret_cast<char*>(tangents.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		bitangents.resize(size);
		file.read(reinterpret_cast<char*>(bitangents.data()), sizeof(DirectX::XMFLOAT3) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		uvs.resize(size);
		file.read(reinterpret_cast<char*>(uvs.data()), sizeof(DirectX::XMFLOAT2) * size);

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		indices.resize(size);
		file.read(reinterpret_cast<char*>(indices.data()), sizeof(UINT) * size);

		resourceMgr->CreateModel<TexturedModel>(name, resourceMgr->GetShader("NormalTexturedObjectShader"), positions, normals, tangents, bitangents, uvs, indices);

	}

	mModel = resourceMgr->GetModel(name);
	DirectX::SimpleMath::Matrix localTransform{};
	file.read(reinterpret_cast<char*>(&localTransform), sizeof(DirectX::XMFLOAT4X4));
	mTransform.SetLocalTransform(localTransform);

	UINT childCount{ 0 };
	file.read(reinterpret_cast<char*>(&childCount), sizeof(UINT));

	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());

	for (UINT i = 0; i < childCount; ++i) {
		auto& child = mChildObjects.emplace_back(std::make_shared<NormalTexturedObject>(resourceMgr, file));
		mTransform.SetChild(&child->GetTransform());
		child->SetMaterial(mMaterials);
	}
}

NormalTexturedObject::~NormalTexturedObject()
{
}

void NormalTexturedObject::UpdateShaderVariables()
{
	GameObject::UpdateShaderVariables();
}

void NormalTexturedObject::Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	GameObject::Render(camera, commandList);
}
