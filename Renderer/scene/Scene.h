#pragma once 
class Scene {
public:
	Scene();
	Scene(const std::string& name);
	~Scene();
	void SetName(const std::string& name);

	const std::string& GetName() const;
	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12CommandQueue>& commandQueue, std::shared_ptr<class Window> window) PURE;
	virtual void Update() PURE;
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList)	PURE;
	virtual void PostRender(ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;
protected:
	std::string mName{ "SceneName" };
};