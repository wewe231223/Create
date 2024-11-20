#pragma once 

// Material 은 별도로 설정해야한다. 
class TexturedObject : public GameObject {
public:
	TexturedObject(std::shared_ptr<ResourceManager>& resourceMgr,const fs::path& binPath);
	TexturedObject(std::shared_ptr<ResourceManager>& resourceMgr, std::ifstream& file);
	virtual ~TexturedObject();
public:
	virtual void UpdateShaderVariables() override;
	virtual void Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList) override;
};