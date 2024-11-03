#pragma once 


class TitleScene : public Scene {
public:
	TitleScene();
	TitleScene(const std::string& name);
	virtual ~TitleScene();

	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window) override;

	virtual void Update();
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList) override;
};