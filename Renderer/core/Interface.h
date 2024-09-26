#pragma once 
//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Interfaces									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

__interface IRendererEntity {
	void WriteContext(void* data) PURE;
};

__interface IGraphicsShader {
	virtual size_t GetShaderID() const PURE;
	virtual void SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;
	virtual bool CheckAttribute(VertexAttribute attribute) PURE;
};

// TODO 
__interface IScene {
	virtual void SetName(const std::string& name) PURE;
	
	virtual UINT GetSceneID() PURE;
	virtual void Load(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;

	virtual MaterialIndex GetMaterial(const std::string& name) PURE;
	virtual TextureIndex GetTexture(const std::string& name) PURE;
	virtual std::shared_ptr<IGraphicsShader> GetShader(const std::string& name) PURE;
	virtual std::shared_ptr<IRendererEntity> GetModel(const std::string& name) PURE;
};