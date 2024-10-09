#include "pch.h"
#include "Game/gameobject/TerrainObject.h"
#include "Game/gameobject/GameObject.h"
#include "Renderer/resource/TerrainImage.h"

TerrainCollider::TerrainCollider(std::shared_ptr<TerrainImage> terrainHeightMap, DirectX::SimpleMath::Vector3 scale)
	: mTerrainHeightMap(terrainHeightMap), mScale(scale)
{
}

TerrainCollider::~TerrainCollider()
{
}

void TerrainCollider::MakeObjectOnTerrain(std::shared_ptr<GameObject> object)
{
	mOnTerrainObject.emplace_back(object);
}

void TerrainCollider::UpdateGameObjectAboveTerrain()
{
    for (auto& object : mOnTerrainObject){
        auto& transform = object->GetTransform();
        float height = mTerrainHeightMap->GetHeight(transform.GetPosition().x, transform.GetPosition().z);
        if (transform.GetPosition().y < height) {
            transform.SetPosition(DirectX::SimpleMath::Vector3(transform.GetPosition().x, height, transform.GetPosition().z));
        }
        
        // Rotate transform's up to match terrain normal
  //      DirectX::SimpleMath::Vector3 terrainNormal = mTerrainHeightMap->GetNormal(static_cast<int>(transform.GetPosition().x),static_cast<int>(transform.GetPosition().z),mScale);

		//DirectX::SimpleMath::Vector3 up = transform.GetUp();
		//DirectX::SimpleMath::Vector3 rotateAxis = up.Cross(terrainNormal);
  //      rotateAxis.Normalize();

  //      if (rotateAxis.Length() < FLT_EPSILON) {
  //          continue;
  //      }

		//float dot = up.Dot(terrainNormal);
		//float angle = acosf(dot);

  //      transform.Rotate(DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(rotateAxis, angle));
        //// a와 b의 외적을 사용하여 회전 축 계산
        //Vector3 rotationAxis = a.Cross(b);
        //rotationAxis.Normalize();

        //// a와 b의 내적을 사용하여 회전 각도 계산
        //float dotProduct = a.Dot(b);
        //float angle = acosf(dotProduct / (a.Length() * b.Length())); // 라디안 단위

        //// 회전 축과 회전 각도를 사용하여 쿼터니언 생성
        //return Quaternion::CreateFromAxisAngle(rotationAxis, angle);
    }
}



TerrainObject::TerrainObject(std::shared_ptr<ResourceManager> resourceMgr, std::shared_ptr<class TerrainImage> terrainImage, DirectX::SimpleMath::Vector3 scale)
{
	resourceMgr->CreateTexture("TerrainBaseTexture", "./Resources/terrain/Base_Texture.dds");
	resourceMgr->CreateTexture("TerrainDetailTexture", "./Resources/terrain/Detail_Texture_7.dds");
    resourceMgr->CreateModel<TerrainModel>("Terrain", resourceMgr->GetShader("TerrainShader"), terrainImage, scale);
	mModel = resourceMgr->GetModel("Terrain");

    Material material{};
    material.DiffuseTexIndex_1 = resourceMgr->GetTexture("TerrainBaseTexture");
	material.DiffuseTexIndex_2 = resourceMgr->GetTexture("TerrainDetailTexture");
	resourceMgr->CreateMaterial("TerrainMaterial", material);

    GameObject::SetMaterial({ resourceMgr->GetMaterial("TerrainMaterial") });

	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());
}

TerrainObject::~TerrainObject()
{
}

void TerrainObject::Update()
{
    GameObject::Update();
}

void TerrainObject::Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    GameObject::Render(camera, commandList);
}
