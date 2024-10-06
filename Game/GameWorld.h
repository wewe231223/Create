#pragma once 
class GameWorld {
public:
	GameWorld(std::shared_ptr<ResourceManager> resourcemgr);
	~GameWorld();

	ResourceManager* GetSceneResource();
	std::span<std::shared_ptr<GameObject>> GetGameObject(EGameObjectTag tag);

	void MakeGameObject(std::shared_ptr<GameObject>&& gameObject);
	void MakeGameObject(EGameObjectTag tag, std::shared_ptr<GameObject>&& gameObject);

	void Awake();
	void Update();
	void PrepareRender(ComPtr<ID3D12GraphicsCommandList>& commandList);
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	std::shared_ptr<ResourceManager> mResource{ nullptr };

	std::array<std::vector<std::shared_ptr<GameObject>>, EGameObjectTag::END> mGameObjectsbyTag{};
	std::vector<std::shared_ptr<GameObject>> mGameObjects{};
};