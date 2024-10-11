#pragma once 

class BinObject : public GameObject {
public:
	BinObject(std::shared_ptr<ResourceManager> resourceMgr,const fs::path& binPath);
	virtual ~BinObject();
public:
private:

};