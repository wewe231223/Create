#pragma once 


class Mesh {
public:
	// 경계값 포함. 
	Mesh(D3D12_PRIMITIVE_TOPOLOGY topology,UINT indexBegin,UINT indexCount,UINT materialIndex);
	~Mesh();

	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList,UINT instance) const;
private:
	D3D12_PRIMITIVE_TOPOLOGY mTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	UINT mIndexBegin{ 0 };
	UINT mIndexCount{ 0 };
	UINT mMaterialIndex{ 0 };
};

