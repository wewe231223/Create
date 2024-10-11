#include "pch.h"
#include "Game/asset/AssetImporter.h"




AssetImporter::AssetImporter(std::shared_ptr<ResourceManager> resourceMgr)
{
}

AssetImporter::~AssetImporter()
{
}

void AssetImporter::Import(const fs::path& path)
{
	std::ifstream file{ path, std::ios::binary };

	assert(file.is_open());

	std::vector<DirectX::XMFLOAT3> mPositions{};
	std::vector<DirectX::XMFLOAT3> mNormals{};
	std::vector<DirectX::XMFLOAT2> mTexCoords{};




}
