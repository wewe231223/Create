#pragma once 
//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Interfaces									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

__interface I3DRenderable {
	virtual DirectX::BoundingOrientedBox GetBoundingBox() const PURE;
	virtual void WriteContext(ModelContext* data,const std::span<MaterialIndex>& materials) PURE;
};

__interface I2DRenderable {
	virtual DirectX::XMFLOAT3X3 GetScreenTransform() PURE;
	virtual void WriteContext(ModelContext2D* data) PURE;
};

__interface IGraphicsShader {
	virtual size_t GetShaderID() const PURE;
	virtual bool SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList, bool shadow = false) PURE;
	virtual bool CheckAttribute(VertexAttribute attribute) PURE;
};

__interface IRendererTransformBase {
	virtual DirectX::SimpleMath::Vector3 GetPosition() const;
	virtual DirectX::SimpleMath::Quaternion GetRotation() const;
	virtual DirectX::SimpleMath::Vector3 GetScale() const;

	virtual DirectX::SimpleMath::Matrix GetWorldMatrix() const;
};

__interface IShadowCasterBase {
	ComPtr<ID3D12Resource>& GetShadowMap() PURE;
	void SetCameraContext(ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;
	void SetShadowTransform(ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;
	void SetFocus(DirectX::SimpleMath::Vector3 focus) PURE;
	void SetPosition(DirectX::SimpleMath::Vector3 position) PURE;
};