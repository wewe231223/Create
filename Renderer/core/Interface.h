#pragma once 
//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Interfaces									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

__interface IRendererEntity {
	virtual DirectX::BoundingOrientedBox GetBoundingBox() const PURE;
	virtual void WriteContext(ModelContext* data,const std::span<MaterialIndex>& materials) PURE;
};

__interface IGraphicsShader {
	virtual size_t GetShaderID() const PURE;
	virtual void SetShader(ComPtr<ID3D12GraphicsCommandList>& commandList) PURE;
	virtual bool CheckAttribute(VertexAttribute attribute) PURE;
};
