#include "pch.h"
#include "Game/gameobjects/MyGameObjects.h"

Terrain::Terrain(std::shared_ptr<ResourceManager> resourcemgr)
{

	resourcemgr->CreateModel<TerrainModel>()
}

Terrain::~Terrain()
{
}
