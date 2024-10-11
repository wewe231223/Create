#pragma once 

// Material 은 별도로 설정해야한다. 
class BinObject : public GameObject {
public:
	BinObject(std::shared_ptr<ResourceManager> resourceMgr,const fs::path& binPath);
	BinObject(std::shared_ptr<ResourceManager> resourceMgr, std::ifstream& file);
	virtual ~BinObject();
public:
	BinObject* GetChild(UINT dfsIndex);
private:
	BinObject* GetChildInternal(UINT& dfsIndex);
private:
	std::vector<BinObject> mChildObjects{};
};