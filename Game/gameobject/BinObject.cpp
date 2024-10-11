#include "pch.h"
#include "Game/gameobject/BinObject.h"

// 내일 실제 데이터 넣어보고 렌더링 해보자
// 에셋 바이너라이저는 단순 정점 정보만 올리고 재질 정보는 별도로 바인딩 하는 것으로 한다. 

BinObject::BinObject(std::shared_ptr<ResourceManager> resourceMgr, const fs::path& binPath)
{

	
}

BinObject::BinObject(std::shared_ptr<ResourceManager> resourceMgr, std::ifstream& file)
{

}

BinObject::~BinObject()
{
}

BinObject* BinObject::GetChild(UINT dfsIndex)
{
	if (dfsIndex == 0) {
		return this;
	}

	BinObject* ret = nullptr;
	UINT index = dfsIndex;


	for (auto& child : mChildObjects) {
		ret = GetChildInternal(dfsIndex);
		if (ret != nullptr) {
			break;
		}
	}


	return ret;
}

BinObject* BinObject::GetChildInternal(UINT& dfsIndex)
{
	if (dfsIndex == 0) {
		return this;
	}

	dfsIndex -= 1;

	BinObject* ret = nullptr;

	for (auto& child : mChildObjects) {
		ret = GetChildInternal(dfsIndex);

		if (ret != nullptr) {
			break;
		}
	}


	return ret;
}


