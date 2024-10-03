#pragma once 

class GameScene : public Scene {
public:
	GameScene();
	GameScene(const std::string& name);
	~GameScene();

	void Update();
	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<Window> window) override;
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList) override;
protected:
	std::shared_ptr<class Camera> mCamera{ nullptr };
	std::shared_ptr<Window> mWindow{ nullptr };
};
