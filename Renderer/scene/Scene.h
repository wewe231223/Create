#pragma once 
class Scene {
public:
	Scene();
	Scene(const std::string& name);
	~Scene();
	void SetName(const std::string& name);

	const std::string& GetName() const;
	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList)		= 0;
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList, UINT memIndex)					= 0;
protected:
	std::string mName{ "SceneName" };
	std::unique_ptr<class ResourceManager> mSceneResource{ nullptr };
};