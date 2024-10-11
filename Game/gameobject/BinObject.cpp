#include "pch.h"
#include "Game/gameobject/BinObject.h"

BinObject::BinObject(std::shared_ptr<ResourceManager> resourceMgr, const fs::path& binPath)
{
	std::string modelName = binPath.stem().string();

	auto model = resourceMgr->GetModel(modelName);

	if (model == nullptr) {

	}

	
}

BinObject::~BinObject()
{
}
