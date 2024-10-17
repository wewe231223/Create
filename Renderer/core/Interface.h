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
	virtual void SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;
	virtual bool CheckAttribute(VertexAttribute attribute) PURE;
};
