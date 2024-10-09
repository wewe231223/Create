#pragma once 

class TerrainCollider {
public:
	TerrainCollider(std::shared_ptr<class TerrainImage> terrainHeightMap,DirectX::SimpleMath::Vector3 scale);
	~TerrainCollider();

	void MakeObjectOnTerrain(std::shared_ptr<class GameObject> object);

	void UpdateGameObjectAboveTerrain();
private:
	std::shared_ptr<TerrainImage> mTerrainHeightMap{ nullptr };
	DirectX::SimpleMath::Vector3 mScale{ 1.0f,1.0f,1.0f };
	
	std::vector<std::shared_ptr<class GameObject>> mOnTerrainObject{};
};


class TerrainObject : public GameObject {
public:
	TerrainObject(std::shared_ptr<ResourceManager> resourceMgr,std::shared_ptr<class TerrainImage> terrainImage, DirectX::SimpleMath::Vector3 scale);
	~TerrainObject();

	void Update() override;
	void Render(std::shared_ptr<Camera> camera,ComPtr<ID3D12GraphicsCommandList>& commandList) override;
};