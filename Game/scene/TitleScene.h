#pragma once 

class TitleScene : public Scene {
public:
	TitleScene();
	TitleScene(const std::string& name);
	virtual ~TitleScene();
public:
	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<Window> window) override;

	virtual void Update();
	virtual void SetShadowCaster(IShadowCasterBase* shadowCaster) override;
	virtual void ShadowCast(ComPtr<ID3D12GraphicsCommandList>& commandList) override;
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList) override;
	virtual void PostRender(ComPtr<ID3D12GraphicsCommandList>& commandList) override;
private:
	std::shared_ptr<Window> mWindow{ nullptr };
	std::shared_ptr<ResourceManager> mResourceManager{ nullptr };
	std::shared_ptr<class Canvas> mCanvas{ nullptr };
};