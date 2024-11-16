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

void TerrainCollider::OnTerrain(std::shared_ptr<class GameObject> object)
{
    auto& transform = object->GetTransform();
    float height = mTerrainHeightMap->GetHeight(transform.GetPosition().x / mScale.x, transform.GetPosition().z / mScale.z);
    transform.SetPosition(DirectX::SimpleMath::Vector3(transform.GetPosition().x, height, transform.GetPosition().z));

}

DirectX::SimpleMath::Vector3 TerrainCollider::OnTerrain(const DirectX::SimpleMath::Vector3& position)
{
    float height = mTerrainHeightMap->GetHeight(position.x / mScale.x, position.z / mScale.z);
    return DirectX::SimpleMath::Vector3(position.x, height, position.z);
}

DirectX::SimpleMath::Vector3 TerrainCollider::GetNormal(const DirectX::SimpleMath::Vector3& position)
{
    int ix = static_cast<int>(position.x / mScale.x);
    int iz = static_cast<int>(position.z / mScale.z);

    float fx = position.x / mScale.x - ix;
    float fz = position.z / mScale.z - iz;


    DirectX::XMFLOAT3 LT{ mTerrainHeightMap->GetNormal(ix,iz + 1,mScale) };
    DirectX::XMFLOAT3 LB{ mTerrainHeightMap->GetNormal(ix, iz, mScale) };
    DirectX::XMFLOAT3 RT{ mTerrainHeightMap->GetNormal(ix + 1, iz + 1, mScale) };
    DirectX::XMFLOAT3 RB{ mTerrainHeightMap->GetNormal(ix + 1, iz, mScale) };


    DirectX::XMFLOAT3 Bot{ DirectX::SimpleMath::Vector3::Lerp(LB, RB, fx) };
    DirectX::XMFLOAT3 Top{ DirectX::SimpleMath::Vector3::Lerp(LT, RT, fx) };

    DirectX::SimpleMath::Vector3 terrainNormal{ DirectX::SimpleMath::Vector3::Lerp(Bot, Top, fz) };
    terrainNormal.Normalize();

	return terrainNormal;
}

bool TerrainCollider::TerrainCollision(Transform& object)
{
    float height = object.GetPosition().y;
	float terrainHeight = mTerrainHeightMap->GetHeight(object.GetPosition().x / mScale.x, object.GetPosition().z / mScale.z);
    
	return height < terrainHeight;
}

void TerrainCollider::UpdateGameObjectAboveTerrain()
{
    for (auto& object : mOnTerrainObject){
        auto& transform = object->GetTransform();
		OnTerrain(object);

		auto pos = transform.GetPosition();

		pos.x = std::clamp(pos.x, 5.f, mTerrainHeightMap->GetWidth() * mScale.x);
		pos.z = std::clamp(pos.z, 5.f, mTerrainHeightMap->GetHeight() * mScale.z);

        transform.SetPosition(pos);

        int ix = static_cast<int>(transform.GetPosition().x / mScale.x);
        int iz = static_cast<int>(transform.GetPosition().z / mScale.z);

        float fx = transform.GetPosition().x / mScale.x - ix;
        float fz = transform.GetPosition().z / mScale.z - iz;


        DirectX::XMFLOAT3 LT{ mTerrainHeightMap->GetNormal(ix,iz + 1,mScale) };
        DirectX::XMFLOAT3 LB{ mTerrainHeightMap->GetNormal(ix, iz, mScale) };
        DirectX::XMFLOAT3 RT{ mTerrainHeightMap->GetNormal(ix + 1, iz + 1, mScale) };
        DirectX::XMFLOAT3 RB{ mTerrainHeightMap->GetNormal(ix + 1, iz, mScale) };


        DirectX::XMFLOAT3 Bot{ DirectX::SimpleMath::Vector3::Lerp(LB, RB, fx) };
        DirectX::XMFLOAT3 Top{ DirectX::SimpleMath::Vector3::Lerp(LT, RT, fx) };

        DirectX::SimpleMath::Vector3 terrainNormal{ DirectX::SimpleMath::Vector3::Lerp(Bot, Top, fz) };
        terrainNormal.Normalize();

		DirectX::SimpleMath::Vector3 up{ transform.GetUp() };


        auto rotation = DirectX::SimpleMath::Quaternion::FromToRotation(up, terrainNormal);
        rotation.Normalize();
        transform.Rotate(rotation);
            
        
        
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

void TerrainCollider::UpdateCameraAboveTerrain(std::shared_ptr<Camera> camera)
{
    auto& transform = camera->GetTransform();

    float height = mTerrainHeightMap->GetHeight(transform.GetPosition().x / mScale.x, transform.GetPosition().z / mScale.z);


    if (transform.GetPosition().y <= height) {
        int ix = static_cast<int>(transform.GetPosition().x / mScale.x);
        int iz = static_cast<int>(transform.GetPosition().z / mScale.z);

        float fx = transform.GetPosition().x / mScale.x - ix;
        float fz = transform.GetPosition().z / mScale.z - iz;


        DirectX::XMFLOAT3 LT{ mTerrainHeightMap->GetNormal(ix,iz + 1,mScale) };
        DirectX::XMFLOAT3 LB{ mTerrainHeightMap->GetNormal(ix, iz, mScale) };
        DirectX::XMFLOAT3 RT{ mTerrainHeightMap->GetNormal(ix + 1, iz + 1, mScale) };
        DirectX::XMFLOAT3 RB{ mTerrainHeightMap->GetNormal(ix + 1, iz, mScale) };


        DirectX::XMFLOAT3 Bot{ DirectX::SimpleMath::Vector3::Lerp(LB, RB, fx) };
        DirectX::XMFLOAT3 Top{ DirectX::SimpleMath::Vector3::Lerp(LT, RT, fx) };

        DirectX::SimpleMath::Vector3 terrainNormal{ DirectX::SimpleMath::Vector3::Lerp(Bot, Top, fz) };


        transform.Translate(terrainNormal * 10.f);

    }

}




TerrainObject::TerrainObject(std::shared_ptr<ResourceManager> resourceMgr, std::shared_ptr<class TerrainImage> terrainImage, DirectX::SimpleMath::Vector3 scale)
{
	resourceMgr->CreateTexture("TerrainBaseTexture", "./Resources/terrain/Base_Texture.dds");
	resourceMgr->CreateTexture("TerrainDetailTexture", "./Resources/terrain/Detail_Texture_7.dds");
    resourceMgr->CreateModel<TerrainModel>("Terrain", resourceMgr->GetShader("TerrainShader"), terrainImage, scale);
	mModel = resourceMgr->GetModel("Terrain");

    Material material{};
    material.Textures[0] = resourceMgr->GetTexture("TerrainBaseTexture");
	material.Textures[1] = resourceMgr->GetTexture("TerrainDetailTexture");
	resourceMgr->CreateMaterial("TerrainMaterial", material);

    GameObject::SetMaterial({ resourceMgr->GetMaterial("TerrainMaterial") });

	mTransform.SetOrientedBoundingBox(mModel->GetBoundingBox());
}

TerrainObject::~TerrainObject()
{
}

void TerrainObject::UpdateShaderVariables()
{
    GameObject::UpdateShaderVariables();
}

void TerrainObject::Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    GameObject::Render(camera, commandList);
}
