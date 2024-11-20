#pragma once 

class NormalTexturedObject : public GameObject {
public:
	NormalTexturedObject(std::shared_ptr<ResourceManager>& resourceMgr, const fs::path& binPath);
	NormalTexturedObject(std::shared_ptr<ResourceManager>& resourceMgr, std::ifstream& file);
	virtual ~NormalTexturedObject();
public:
	virtual void UpdateShaderVariables() override;
	virtual void Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList) override;
};