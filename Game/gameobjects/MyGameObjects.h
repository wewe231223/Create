#pragma once 

class Terrain : public GameObject {
public:
	Terrain(std::shared_ptr<ResourceManager> resourcemgr);
	virtual ~Terrain();
};