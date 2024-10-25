#pragma once 

class Bullet : public GameObject {
public:
	Bullet(std::shared_ptr<I3DRenderable> model, const std::vector<MaterialIndex>& materials);
	~Bullet();
public:
	void Reset(DirectX::SimpleMath::Vector3 dir);
	bool Validate() const ;
	virtual void UpdateShaderVariables() override;
private:
	DirectX::SimpleMath::Vector3 mDirection{};

	float mTimeOut{ 5.f };
};


class GameScene : public Scene {
public:
	GameScene();
	GameScene(const std::string& name);
	virtual ~GameScene();

	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window) override;
	
	void Update();
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList) override;
private:
	void LoadTextures();
	void CreateMaterials();
	void InitCamera(ComPtr<ID3D12Device>& device,ComPtr<ID3D12GraphicsCommandList>& commandList);
	void InitSkyBox(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList);
	void InitTerrain(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList);
	void InitCameraMode();

	void UpdateShaderVariables();
private:
	std::shared_ptr<ResourceManager> mResourceManager{ nullptr };
	std::shared_ptr<UIRenderer> mUIRenderer{ nullptr };

	std::shared_ptr<Window> mWindow{ nullptr };

	std::shared_ptr<Camera> mMainCamera{ nullptr };
	std::shared_ptr<class CameraMode> mCurrentCameraMode{ nullptr };
	std::array<std::shared_ptr<class CameraMode>, CT_END> mCameraModes{ nullptr };

	std::shared_ptr<GameObject> mPlayer{ nullptr };

	std::vector<std::shared_ptr<GameObject>> mGameObjects{};


	// 총알 오브젝트 만들기... 
	ObjectPool<Bullet, 64> mBullets{};

	std::shared_ptr<class TerrainCollider> mTerrain{ nullptr };
};