#pragma once 

class GameScene : public Scene {
public:
	GameScene();
	GameScene(const std::string& name);
	~GameScene();

	void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList) override;
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList,IFrameMemory* frameMemory) override;
};