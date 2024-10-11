#pragma once 

// 애는 그냥 폴더 안에 있는 모든 bin 파일을 읽게 하자 

class AssetImporter {
public:
	AssetImporter(std::shared_ptr<ResourceManager> resourceMgr);
	~AssetImporter();

	void Import(const fs::path& path);
};