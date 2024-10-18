#pragma once 

class GameScene : public Scene{
public:
	GameScene();
	GameScene(const std::string& name);
	virtual ~GameScene();

	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window) override;
	
	void Update();
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList) override;
private:
	void InitCamera();
	void InitSkyBox();

	void UpdateShaderVariables();
private:
	std::shared_ptr<ResourceManager> mResourceManager{ nullptr };
	std::shared_ptr<UIRenderer> mUIRenderer{ nullptr };

	std::shared_ptr<I3DRenderable> mSkyBox{ nullptr };
	std::vector<MaterialIndex> mSkyBoxMaterials{};
	
	std::shared_ptr<Camera> mMainCamera{ nullptr };
	std::shared_ptr<class CameraMode> mCurrentCameraMode{ nullptr };
	std::array<std::shared_ptr<class CameraMode>, CT_END> mCameraModes{ nullptr };

	std::vector<std::shared_ptr<class GameObject>> mGameObjects{};

	std::shared_ptr<class TerrainCollider> mTerrain{ nullptr };
};