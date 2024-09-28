#pragma once 

class GameScene : public Scene {
public:
	GameScene();
	GameScene(const std::string& name);
	~GameScene();

	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList) override;
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList) override;

	std::shared_ptr<IRendererEntity> nt{ nullptr };
};