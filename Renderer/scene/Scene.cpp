#include "pch.h"
#include "scene/Scene.h"
#include "scene/ResourceManager.h"
#include "buffer/DefaultBuffer.h"
Scene::Scene()
{
}

Scene::Scene(const std::string& name)
{
	mName = name;
}

Scene::~Scene()
{
}

void Scene::SetName(const std::string& name)
{
	mName = name;
}

const std::string& Scene::GetName() const
{
	return mName;
}

