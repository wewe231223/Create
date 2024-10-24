#pragma once 

class CubeObject : public GameObject {
public:
	CubeObject(std::shared_ptr<ResourceManager> resourceMgr);
	~CubeObject();

	void UpdateShaderVariables() override;
	void Render(std::shared_ptr<Camera> camera, ComPtr<ID3D12GraphicsCommandList>& commandList) override;
};